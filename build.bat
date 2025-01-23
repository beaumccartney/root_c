:: wraps the python script and forwards all arguments to save typing python3 ...
@echo off
pushd "%~dp0"

python3 build-windows.py %*

popd
