@echo off
setlocal enabledelayedexpansion

pushd build

cl /nologo /Z7 /I..\layers ..\scratch\main.c
main

popd
