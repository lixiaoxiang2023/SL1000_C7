@echo off

rem for lib vs app install
rem author:xuqiang
rem date:2025.02.21

rem get current dir
set ROOT=%~dp0
cd /d %ROOT%

set os="win";

if not exist ".dependlibs" ( mkdir .dependlibs )

rem update scritp files
cd .dependlibs
echo Update install libs script files.

if not exist plat_script (
    call :CloneOrCheckoutFromRepo
) else (
    if not exist .\..\winbuild_debug.txt (
        if exist .\plat_script ( rmdir /s/q .\plat_script )
        call :CloneOrCheckoutFromRepo
    )
)

rem check files
if not exist .\plat_script\__install_libs_win.bat (
    echo ***Stript files __install_libs_win.bat not exist.
    exit (1)
)

rem start
if exist .\plat_script\__install_libs_win.bat (
    call .\plat_script\__install_libs_win.bat "%ROOT%" "%os%"
)

cd "%ROOT%"
goto :EOF

rem checkout
:CloneOrCheckoutFromRepo

git clone ssh://git@192.168.200.46:53802/sw_frame_utility/plat_script.git plat_script
if %errorlevel% neq 0 (
    git clone http://192.168.200.46:53800/sw_frame_utility/plat_script.git plat_script
    if %errorlevel% neq 0 (
        svn export http://172.20.22.225:8080/svn/libs/script_v2 plat_script
    )
)

exit /b