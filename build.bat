@echo off
setlocal enabledelayedexpansion
cd /D "%~dp0"

for %%a in (%*) do set "%%a=1"
if not "%release%"=="1" set debug=1
if "%debug%"=="1"       set release=0 && echo [debug mode]
if "%release%"=="1"     set debug=0 && echo [release mode]

:: common cl flags
set flags=/I..\layers\ /I..\local\ /I..\thirdparty\ /nologo /Z7 /FC -Gm- /W4 /wd4100 /wd4201

if "%debug%"=="1"        set flags=%flags% /Od /Ob1 /DBUILD_DEBUG=1
if "%release%"=="1"      set flags=%flags% /O2 /DBUILD_DEBUG=0
if "%profile%"=="1"      set superluminal=1
if "%superluminal%"=="1" set flags=%flags% -DPROFILE_SUPERLUMINAL=1 && (echo [superluminal profiling enabled])
if "%tracy%"=="1"        set flags=%flags% -DPROFILE_TRACY=1 /MD    && (echo [tracy profiling enabled])
if "%asan%"=="1"         set flags=%flags% -fsanitize=address       && (echo [asan enabled])

:: RTCu will trip on asan itself if its on, so only turn it on if no asan
if "%RTC%"=="1"   (if "%asan%"=="1" set flags=%flags% /RTCsu && (echo [RTC enabled]) else (set flags=%flags% /RTCcsu))

if not exist build mkdir build
if not exist local mkdir local

pushd build

if exist *.pdb del *.pdb

cl %flags% /Fe:local ..\local\main.c /link /DEBUG:FULL /MANIFEST:EMBED /INCREMENTAL:NO

popd
