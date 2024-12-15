:: script to start rad debugger with appropriate local stuff
@echo off
cd /D "%~dp0"

start raddbg --project:.\root_c.raddbg_project
