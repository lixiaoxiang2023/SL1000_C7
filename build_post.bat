@echo off& setlocal enabledelayedexpansion 

set DIR_PROJ=%1%
set NAME_TARGET_IN=%2%
set DIR_OBJ=%DIR_PROJ%\Debug\Obj
set DIR_BIN=%DIR_PROJ%\Debug\Exe

cd /d "%DIR_PROJ%"
rem get names
for /f "tokens=1,2 delims=." %%a in ("%NAME_TARGET_IN%") do (
    set NAME_TARGET_BODY=%%a
)

rem cheak icf type, if if not exit <state>$PROJ_DIR$\link_only_app.icf</state> return, no operations
find "<state>$PROJ_DIR$\link_only_app.icf</state>" "%NAME_TARGET_BODY%.ewp" 1>nul 2>nul
if %errorlevel% neq 0 (
    echo no find
    goto outFlag
)

rem get obj files, and remove startup_xx files
cd /d %DIR_OBJ%
set "pattern=startup"
for /f %%i in ('dir /B *.o') do (
    set "line=%%i"
    if "!line:%pattern%=!"=="!line!" (
        set O_FILES=!O_FILES! %%i
    )
)

rem generate bin
set NAME_TARGET=%NAME_TARGET_BODY%.bin
if exist "%DIR_BIN%\%NAME_TARGET%" (
   copy /Y "%DIR_BIN%\%NAME_TARGET%" "%DIR_BIN%\app.bin"
) else (
   goto outFlag
)

set UC_FLAG=%O_FILES%
set UC_FLAG=%UC_FLAG% --map %DIR_PROJ%\Debug\Exe\boot.map
set UC_FLAG=%UC_FLAG% --config %DIR_PROJ%\link_auto_link.icf
set UC_FLAG=%UC_FLAG% --semihosting
set UC_FLAG=%UC_FLAG% --entry bootMain
set UC_FLAG=%UC_FLAG% --vfe
set UC_FLAG=%UC_FLAG% -o %DIR_BIN%\auto_build.out
set UC_FLAG=%UC_FLAG% --keep zzapp
set UC_FLAG=%UC_FLAG% --image_input=%DIR_BIN%\app.bin,zzapp,.section_app,4

rem echo %UC_FLAG%
ilinkarm.exe %UC_FLAG%
if %errorlevel% neq 0 (
   exit (100)
)

cd /d %DIR_BIN%
ielftool.exe --bin auto_build.out %NAME_TARGET_BODY%.bin
if %errorlevel% neq 0 (
   exit (100)
)

del -fr app.bin 1>nul 2>nul
del -fr auto_build.out 1>nul 2>nul

rem back up bin
cd %DIR_PROJ%
if not exist  %NAME_TARGET_BODY%bin (
    mkdir %NAME_TARGET_BODY%bin
)

copy /Y %DIR_BIN%\%NAME_TARGET_BODY%.bin %NAME_TARGET_BODY%bin\%NAME_TARGET_BODY%.bin

:outFlag
rem exit (0)