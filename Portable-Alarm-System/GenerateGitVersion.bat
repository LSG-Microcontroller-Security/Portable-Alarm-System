@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "REPO=%~dp0.."
set "OUT=%~dp0GitVersion.h"
set "TMP=%~dp0GitVersion.tmp"

rem Verify Git repository
git -C "%REPO%" rev-parse --is-inside-work-tree >nul 2>&1
if errorlevel 1 (
	echo ERROR: Git repository not found.
	exit /b 1
)

rem Verify current branch
for /f "delims=" %%B in ('git -C "%REPO%" rev-parse --abbrev-ref HEAD') do set "BRANCH=%%B"

if /I not "!BRANCH!"=="release" (
	echo ERROR: Firmware release can only be built from branch release.
	echo Current branch: !BRANCH!
	exit /b 1
)

rem Verify working tree is clean
set "DIRTY="
for /f "delims=" %%S in ('git -C "%REPO%" status --porcelain') do set "DIRTY=1"

if defined DIRTY (
	echo ERROR: Working tree is not clean.
	echo Commit or discard changes before building the release.
	exit /b 1
)

rem Update origin/release
git -C "%REPO%" fetch origin release --quiet
if errorlevel 1 (
	echo ERROR: Cannot fetch origin/release.
	exit /b 1
)

rem Compare local HEAD with origin/release
for /f "delims=" %%H in ('git -C "%REPO%" rev-parse HEAD') do set "LOCAL_HASH=%%H"
for /f "delims=" %%H in ('git -C "%REPO%" rev-parse FETCH_HEAD') do set "REMOTE_HASH=%%H"

if /I not "!LOCAL_HASH!"=="!REMOTE_HASH!" (
	echo ERROR: Local release is not aligned with origin/release.
	echo Local : !LOCAL_HASH!
	echo Origin: !REMOTE_HASH!
	exit /b 1
)

rem Get short Git hash
for /f "delims=" %%H in ('git -C "%REPO%" rev-parse --short=8 HEAD') do set "GIT_HASH=%%H"

rem Generate header
> "%TMP%" echo #pragma once
>> "%TMP%" echo #define GIT_VERSION "!GIT_HASH!"

rem Replace header only when content changed
if exist "%OUT%" (
	fc /b "%TMP%" "%OUT%" >nul 2>&1
	if not errorlevel 1 (
		del "%TMP%"
		echo Git version: !GIT_HASH!
		exit /b 0
	)
)

move /y "%TMP%" "%OUT%" >nul

echo Git version: !GIT_HASH!
exit /b 0
