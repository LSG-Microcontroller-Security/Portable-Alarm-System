@echo off
setlocal EnableExtensions

set "REPO=%~dp0.."
set "OUT=%~dp0GitVersion.h"
set "TMP=%~dp0GitVersion.tmp"
set "GITTMP=%TEMP%\PortableAlarmGitVersion.txt"

git -C "%REPO%" rev-parse --is-inside-work-tree >nul 2>&1
if errorlevel 1 (
	echo ERROR: Git repository not found.
	exit /b 1
)

git -C "%REPO%" rev-parse --abbrev-ref HEAD > "%GITTMP%"
set /p BRANCH=<"%GITTMP%"

if /I not "%BRANCH%"=="release" (
	echo ERROR: Firmware release can only be built from branch release.
	echo Current branch: %BRANCH%
	del "%GITTMP%" >nul 2>&1
	exit /b 1
)

git -C "%REPO%" status --porcelain -- . ":(exclude)Portable-Alarm-System/BasicPortableAlarmSystem.vcxproj" > "%GITTMP%"
for %%A in ("%GITTMP%") do set SIZE=%%~zA

if not "%SIZE%"=="0" (
	echo ERROR: Working tree is not clean.
	echo Commit or discard changes before building the release.
	del "%GITTMP%" >nul 2>&1
	exit /b 1
)

git -C "%REPO%" fetch origin release --quiet
if errorlevel 1 (
	echo ERROR: Cannot fetch origin/release.
	del "%GITTMP%" >nul 2>&1
	exit /b 1
)

git -C "%REPO%" rev-parse HEAD > "%GITTMP%"
set /p LOCAL_HASH=<"%GITTMP%"

git -C "%REPO%" rev-parse FETCH_HEAD > "%GITTMP%"
set /p REMOTE_HASH=<"%GITTMP%"

if /I not "%LOCAL_HASH%"=="%REMOTE_HASH%" (
	echo ERROR: Local release is not aligned with origin/release.
	echo Local : %LOCAL_HASH%
	echo Origin: %REMOTE_HASH%
	del "%GITTMP%" >nul 2>&1
	exit /b 1
)

git -C "%REPO%" rev-parse --short=7 HEAD > "%GITTMP%"
set /p GIT_HASH=<"%GITTMP%"

> "%TMP%" echo #pragma once
>> "%TMP%" echo #define GIT_VERSION "%GIT_HASH%"

move /y "%TMP%" "%OUT%" >nul
del "%GITTMP%" >nul 2>&1

echo Git version: %GIT_HASH%
exit /b 0