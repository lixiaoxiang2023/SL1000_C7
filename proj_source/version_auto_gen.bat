echo off
setlocal enabledelayedexpansion

rem for plat version auto generating.
rem author:xuqiang
rem date:2023.03.04

rem get current dir
set ROOT=%~dp0
cd /d %ROOT%

rem search folders
rem dir /ad /b
echo %CD%
if not exist "version_json" (
    goto outflag
)

for /f "delims==" %%a in ('dir /b /s .\version_json\*.json') do (
    .\.dependlibs\versiontool\xversion.exe "%%a"

    if %errorlevel%==0 (
		echo version auto gen: ok.
	) else (
        echo version auto gen: error.
    )

    goto outflag
)

rem pause
:outflag
