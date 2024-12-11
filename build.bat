@echo off
setlocal enabledelayedexpansion
cd /D "%~dp0"

for %%a in (%*) do set "%%a=1"
if not "%release%"=="1" set debug=1
if "%debug%"=="1"   set release=0 && echo [debug mode]
if "%release%"=="1" set debug=0 && echo [release mode]

:: common cl flags
set flags=/I..\layers\ /I..\scratch\ /nologo /Z7 /FC -Gm- /W4 /wd4100 /wd4201

if "%debug%"=="1" set flags=%flags% /Od /Ob1 /DBUILD_DEBUG=1
if "%asan%"=="1"  set flags=%flags% -fsanitize=address && (echo [asan enabled])

:: RTCu will trip on asan itself if its on, so only turn it on if no asan
if "%RTC%"=="1"   (if "%asan%"=="1" set flags=%flags% /RTCsu && (echo [RTC enabled]) else (set flags=%flags% /RTCcsu))

if not exist build mkdir build
if not exist scratch mkdir scratch

pushd build

del *.pdb

@echo on
cl %flags% ..\apps\test\main.c /link /MANIFEST:EMBED /INCREMENTAL:NO /pdbaltpath:%%%%_PDB%%%%
@echo off

popd
