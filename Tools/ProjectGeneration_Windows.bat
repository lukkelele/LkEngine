@echo off
REM Navigate to the LkEngine root directory.
cd /d "%~dp0\.."

REM Run Premake from the new directory.
call External\premake-5.0\premake5.exe --file=LkEngine.lua vs2022