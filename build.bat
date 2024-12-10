REM TODO(beau):
REM  single flags accumulator
REM  CL flags from https://nullprogram.com/blog/2023/04/29/

@echo off
setlocal enabledelayedexpansion
cd /D "%~dp0"

for %%a in (%*) do set "%%a=1"
if not "%release%"=="1" set debug=1
if "%debug%"=="1"   set release=0 && echo [debug mode]
if "%release%"=="1" set debug=0 && echo [release mode]

set auto_compile_flags=
if "%asan%"=="1" set auto_compile_flags=%auto_compile_flags% -fsanitize=address && echo [asan enabled]

set cl_common= /I..\layers\ /I..\scratch\ /nologo /Z7
set cl_debug= /Od /Ob1 /DBUILD_DEBUG=1 %cl_common%

set build_kind_flags=
if "%debug%"=="1" set build_kind_flags=%cl_debug%

if not exist build mkdir build
if not exist scratch mkdir scratch

pushd build

del *.pdb
cl %cl_common% %build_kind_flags% %auto_compile_flags% ..\apps\test\main.c /link /MANIFEST:EMBED /INCREMENTAL:NO /pdbaltpath:%%%%_PDB%%%%

popd
