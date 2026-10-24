# This script is installed beside GenerateGitVersion.bat. Avoid command-line
# arguments: Visual Micro's prebuild command processing may drop a trailing
# parameter. Resolve both paths from the script's actual directory instead.
$SketchPath = $PSScriptRoot
$GeneratedHeader = Join-Path $SketchPath 'GitVersion.generated.tmp'
$ErrorActionPreference = 'Stop'
$localLibraryLinkError = 'GPM_AVR_LOCAL_LIBRARY_LINK_ERROR'

function Invoke-Git([string]$Location, [string[]]$Arguments) {
    $value = & git -C $Location @Arguments 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "Git command failed in $($Location): git $($Arguments -join ' ')`n$($value -join "`n")"
    }
    return ($value -join "`n").Trim()
}
# The GUI is available only for a local, interactive Windows build.
# Keeping this as a separate function also allows headless regression tests
# to substitute a decision without weakening the production safety checks.
function Request-DirtyBuildApproval([string]$Message) {
    if ($env:CI -or $env:GITHUB_ACTIONS -or -not [Environment]::UserInteractive -or
        [Environment]::OSVersion.Platform -ne [PlatformID]::Win32NT) {
        throw "$Message`r`nBuild stopped: interactive confirmation is unavailable."
    }
    try {
        Add-Type -AssemblyName System.Windows.Forms
        $answer = [System.Windows.Forms.MessageBox]::Show(
            "$Message`r`n`r`nThese local changes are not committed to Git and may make this build unreproducible.`r`n`r`nDo you want to continue the build anyway?",
            'GPM - WARNING: Uncommitted Git Changes',
            [System.Windows.Forms.MessageBoxButtons]::YesNo,
            [System.Windows.Forms.MessageBoxIcon]::Warning,
            [System.Windows.Forms.MessageBoxDefaultButton]::Button2
        )
        return ($answer -eq [System.Windows.Forms.DialogResult]::Yes)
    }
    catch { throw "$Message`r`nCannot display confirmation: $($_.Exception.Message)" }
}
function Normalize-Remote([string]$Remote) {
    $url = $Remote.Trim()
    if ($url -match '^git@github\.com:(.+)$') { $url = "https://github.com/$($Matches[1])" }
    elseif ($url -match '^ssh://git@github\.com/(.+)$') { $url = "https://github.com/$($Matches[1])" }
    return ($url -replace '\.git$', '').TrimEnd('/')
}
function Write-Utf8([string]$Path, [string]$Text) {
    [System.IO.File]::WriteAllText($Path, $Text, (New-Object System.Text.UTF8Encoding($false)))
}
try {
    $repo = Invoke-Git $SketchPath @('rev-parse', '--show-toplevel')
    $gitDir = Invoke-Git $repo @('rev-parse', '--absolute-git-dir')
    $branch = Invoke-Git $repo @('branch', '--show-current')
    $manifestFile = Join-Path $repo '.gpm/avr-libraries.json'
    if (-not (Test-Path -LiteralPath $manifestFile)) {
        throw "Missing AVR dependency manifest: $manifestFile"
    }
    $manifest = Get-Content -LiteralPath $manifestFile -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($manifest.schemaVersion -ne 1) { throw 'Unsupported AVR dependency manifest schema' }
    $repositories = @($manifest.gitRepositories | Where-Object { $null -ne $_ })
    $mappingFile = Join-Path $gitDir 'gpm-avr-library-paths.json'
    if ($repositories.Count -gt 0 -and -not (Test-Path -LiteralPath $mappingFile)) {
        throw $localLibraryLinkError
    }
    $localSources = @()
    if (Test-Path -LiteralPath $mappingFile) {
        $mapping = Get-Content -LiteralPath $mappingFile -Raw -Encoding UTF8 | ConvertFrom-Json
        $localSources = @($mapping.sources | Where-Object { $null -ne $_ })
    }
    $changed = $false
    $developmentBranch = $branch -match '^(develop|feature/.+|fix/.+|bugfix/.+|task/.+|hotfix/.+)$'
    $hasDirtyLibraries = $false
    $dirtyNames = New-Object 'System.Collections.Generic.List[string]'
    $firmwareStatus = Invoke-Git $repo @('status', '--porcelain', '--', '.', ':(exclude)**/GitVersion.generated.h', ':(exclude,glob)**/*.vcxproj')
    if ($firmwareStatus) { $dirtyNames.Add((Split-Path $repo -Leaf)) }
    $localRevisions = @()
    $pendingUpdates = @()
    $revisionErrors = @()
    foreach ($source in $repositories) {
        $candidates = @($localSources | Where-Object {
            (Normalize-Remote $_.url) -ieq (Normalize-Remote $source.url)
        })
        if ($candidates.Count -ne 1) {
            throw $localLibraryLinkError
        }
        try { $libraryRoot = [System.IO.Path]::GetFullPath($candidates[0].path) }
        catch { throw $localLibraryLinkError }
        if (-not (Test-Path -LiteralPath $libraryRoot -PathType Container)) {
            throw $localLibraryLinkError
        }
        try { $actualRoot = Invoke-Git $libraryRoot @('rev-parse', '--show-toplevel') }
        catch {
            if (-not (Test-Path -LiteralPath (Join-Path $libraryRoot '.git'))) { throw $localLibraryLinkError }
            throw
        }
        if ([System.IO.Path]::GetFullPath($actualRoot) -ine $libraryRoot) {
            throw $localLibraryLinkError
        }
        $origin = Invoke-Git $libraryRoot @('remote', 'get-url', 'origin')
        if ((Normalize-Remote $origin) -ine (Normalize-Remote $source.url)) {
            throw "Wrong Git remote for $libraryRoot ($origin, expected $($source.url))"
        }
        $status = Invoke-Git $libraryRoot @('status', '--porcelain')
        if ($status) {
            $dirtyNames.Add((Split-Path $libraryRoot -Leaf))
            $hasDirtyLibraries = $true
        }
        $head = Invoke-Git $libraryRoot @('rev-parse', 'HEAD')
        if ($head -notmatch '^[a-fA-F0-9]{40}$') { throw "Invalid local Git SHA for $libraryRoot" }
        $localRevisions += [pscustomobject]@{ url = $source.url; ref = $head }
        if ($source.ref -ne $head) {
            if (-not $developmentBranch) {
                $revisionErrors += "Library version mismatch on protected/non-development branch '$branch'. Expected $($source.ref), local $head. Update and commit the manifest on a development branch first."
            }
            else {
                $pendingUpdates += [pscustomobject]@{ source = $source; head = $head }
            }
        }
        else { Write-Host "AVR libraries unchanged: $head" }
    }
    # Check all participating repositories together, before updating the manifest.
    if ($dirtyNames.Count -gt 0) {
        $names = ($dirtyNames | Select-Object -Unique) -join "`r`n- "
        $message = "WARNING! GPM detected uncommitted changes in the following Git repositories:`r`n- $names"
        if (-not $developmentBranch) {
            throw "$message`r`nBuild stopped: this is a protected or non-development branch."
        }
        if (-not (Request-DirtyBuildApproval $message)) {
            throw 'Build cancelled by user: uncommitted Git changes were not approved.'
        }
        Write-Warning "Uncommitted Git changes approved for build: $($dirtyNames -join ', ')"
    }
    if ($revisionErrors.Count -gt 0) {
        throw ($revisionErrors -join "`n")
    }
    # Never record a partial set of local commits while any library has
    # uncommitted changes. Only clean development checkouts update the manifest.
    if ($hasDirtyLibraries) {
        if ($pendingUpdates.Count -gt 0) {
            Write-Warning 'The AVR library manifest was NOT updated because local libraries are dirty. Commit the libraries, then rebuild before opening a firmware PR.'
        }
    }
    else {
        foreach ($update in $pendingUpdates) {
            Write-Host "AVR libraries: $($update.source.url) $($update.source.ref) -> $($update.head)"
            $update.source.ref = $update.head
            $changed = $true
        }
    }
    if ($changed) {
        $json = $manifest | ConvertTo-Json -Depth 30
        Write-Utf8 $manifestFile ($json + "`n")
        Write-Host 'Updated .gpm/avr-libraries.json: commit it with your firmware before opening the PR.'
    }
    # The generated header is ignored by Git. Its paired version references
    # are reproducible from the committed firmware and library manifest.
    $versions = @()
    foreach ($revision in $localRevisions) { $versions += "$($revision.url)@$($revision.ref)" }
    foreach ($lib in @($manifest.arduinoLibraryManager | Where-Object { $null -ne $_ })) {
        if ([string]::IsNullOrWhiteSpace($lib.version)) { throw "Unpinned Arduino Library Manager version: $($lib.name)" }
        $versions += "$($lib.name)@$($lib.version)"
    }
    # Same seven-character interface as GIT_VERSION, including on development branches.
    # One Git repository: its abbreviated commit. Multiple dependencies: a
    # deterministic seven-character fingerprint of all pinned references.
    # Always use a real Git SHA when exactly one Git repository is pinned.
    # Multiple dependencies (or none) use a deterministic SHA256 fingerprint;
    # an empty dependency set has the SHA256(empty string) fingerprint.
    if ($versions.Count -eq 1 -and $repositories.Count -eq 1) {
        $shortVersion = ([string]$localRevisions[0].ref).Substring(0, 7).ToLowerInvariant()
    }
    else {
        $data = [System.Text.Encoding]::UTF8.GetBytes(($versions -join ';'))
        $sha256 = [System.Security.Cryptography.SHA256]::Create()
        try { $digest = $sha256.ComputeHash($data) }
        finally { $sha256.Dispose() }
        $shortVersion = [System.BitConverter]::ToString($digest).Replace('-', '').Substring(0, 7).ToLowerInvariant()
    }
    # The suffix describes the build origin, independently of the library revision.
    if ($branch -notin @('main', 'master')) { $shortVersion += '*' }
    if ($hasDirtyLibraries) { $shortVersion += '-dirty' }
    $headerLine = "#define GIT_LIBRARIES_VERSION `"$shortVersion`"`n"
    [System.IO.File]::AppendAllText($GeneratedHeader, $headerLine, (New-Object System.Text.UTF8Encoding($false)))
    Write-Host "GIT_LIBRARIES_VERSION: $shortVersion"
    Write-Host "AVR library revision(s): $($versions -join '; ')"
    exit 0
}
catch {
    if ($_.Exception.Message -eq $localLibraryLinkError) {
        [Console]::Error.WriteLine("ERROR: AVR local libraries not linked or path invalid.")
        [Console]::Error.WriteLine("Run Git Project Manager (GPM) to configure library paths, then rebuild.")
    }
    else {
        [Console]::Error.WriteLine("GPM AVR library version error: $($_.Exception.Message)")
    }
    exit 1
}
