@echo off
setlocal EnableExtensions

set "REPO=%~dp0.."
set "BUILD_PATH=%~1"
set "OUT=%BUILD_PATH%\GitVersion.h"
set "TMP=%BUILD_PATH%\GitVersion.tmp"
set "GITTMP=%TEMP%\PortableAlarmGitVersion.txt"

if "%BUILD_PATH%"=="" (
	echo ERROR: Build path not provided.
	exit /b 1
)

git -C "%REPO%" rev-parse --is-inside-work-tree >nul 2>&1
if errorlevel 1 (
	call :fail "Git repository not found."
	exit /b 1
)

git -C "%REPO%" rev-parse --abbrev-ref HEAD > "%GITTMP%"
set /p BRANCH=<"%GITTMP%"

if /I not "%BRANCH%"=="release" (
	call :fail "Current branch is not release."
	echo Current branch: %BRANCH%
	exit /b 1
)

git -C "%REPO%" status --porcelain -- . ":(exclude)Portable-Alarm-System/BasicPortableAlarmSystem.vcxproj" > "%GITTMP%"
for %%A in ("%GITTMP%") do set SIZE=%%~zA

if not "%SIZE%"=="0" (
	call :fail "Working tree is not clean."
	exit /b 1
)

git -C "%REPO%" fetch origin release --quiet
if errorlevel 1 (
	call :fail "Cannot fetch origin/release."
	exit /b 1
)

git -C "%REPO%" rev-parse HEAD > "%GITTMP%"
set /p LOCAL_HASH=<"%GITTMP%"

git -C "%REPO%" rev-parse FETCH_HEAD > "%GITTMP%"
set /p REMOTE_HASH=<"%GITTMP%"

if /I not "%LOCAL_HASH%"=="%REMOTE_HASH%" (
	call :fail "Local release is not aligned with origin/release."
	echo Local : %LOCAL_HASH%
	echo Origin: %REMOTE_HASH%
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

:fail
> "%OUT%" echo #pragma once
>> "%OUT%" echo #error INVALID_RELEASE_BUILD
del "%GITTMP%" >nul 2>&1
echo ERROR: %~1
exit /b 0