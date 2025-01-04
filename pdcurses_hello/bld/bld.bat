rem @echo off
:: Run
::  cmake -G "???" -DCMAKE_TOOLCHAIN_FILE=toolchain/???-toolchain.cmake -B bld/??? .
::  cmake --build bld/???
:: corresponding to the toolchain name in the argument.
pushd %~dp0
cd ..

set Toolchain=%1
set GENE=
set COMPILER=
set ARCH=

if "%Toolchain%"=="" goto ERR_TOOLCHAIN_LIST
if not exist toolchain\%Toolchain%-toolchain.cmake goto ERR_TOOLCHAIN

for /f "tokens=1,2 delims=-" %%a in ("%Toolchain%") do (
    set "COMPILER=%%a"
    set "ARCH=%%b"
)

if not exist thirdparty\lib\%Toolchain%* call thirdparty\install_pdcurses.bat %COMPILER% %ARCH%

set OPT1=-DCMAKE_BUILD_TYPE=Release
set OPT2=

if /I "%COMPILER%"=="watcom"  set GENE=-G "Watcom WMake"
if /I "%COMPILER%"=="djgpp"   set GENE=-G "MinGW Makefiles"
if /I "%COMPILER%"=="mingw"   set GENE=-G "MinGW Makefiles"
if /I "%COMPILER%"=="borland" set GENE=-G "Borland Makefiles"

if /I not "%COMPILER%"=="vc" goto L_CMAKE
set OPT1=
set OPT2=--config Release
set ARCH1=
set "ARCH2=-A Win32"
if /I "%ARCH%"=="win32" goto SKIP_ARCH
set "ARCH1= Win64"
set "ARCH2=-A x64"
:SKIP_ARCH
set GENE=-G "NMake Makefiles"
@if /I not "%PATH:Microsoft Visual Studio 12.0=%"=="%PATH%" set GENE=-G "Visual Studio 12 2013%ARCH1%"
@if /I not "%PATH:Microsoft Visual Studio 14.0=%"=="%PATH%" set GENE=-G "Visual Studio 14 2015%ARCH1%"
@if /I not "%PATH:Microsoft Visual Studio\2017=%"=="%PATH%" set GENE=-G "Visual Studio 15 2017%ARCH1%"
@if /I not "%PATH:Microsoft Visual Studio\2019=%"=="%PATH%" set GENE=-G "Visual Studio 16 2019" %ARCH2%
@if /I not "%PATH:Microsoft Visual Studio\2022=%"=="%PATH%" set GENE=-G "Visual Studio 17 2022" %ARCH2%


:L_CMAKE
cmake %GENE% -DCMAKE_TOOLCHAIN_FILE=toolchain/%Toolchain%-toolchain.cmake %OPT1% -B bld/%Toolchain% .
cmake --build bld/%Toolchain% %OPT2%

:: Copy *.exe to bin/
if not exist bin\%Toolchain% mkdir bin\%Toolchain%
if exist bin\%Toolchain%\*.exe del bin\%Toolchain%\*.exe
if exist bld\%Toolchain%\*.exe copy /b bld\%Toolchain%\*.exe bin\%Toolchain%\
if exist bld\%Toolchain%\release\*.exe copy /b bld\%Toolchain%\release\*.exe bin\%Toolchain%\

goto END

:ERR_TOOLCHAIN
@echo ERROR: No toolchain : %Toolchain%
@echo:
:ERR_TOOLCHAIN_LIST
@echo Usage: bld [TOOLCHAIN]
@for %%a in (toolchain\*.cmake) do @call :PUT_TOOLCHAIN_NAME %%a
goto END

:PUT_TOOLCHAIN_NAME
@set NAME=%1
@set NAME=%NAME:-toolchain.cmake=%
@set NAME=%NAME:toolchain\=%
@echo 	%NAME%
@exit /b 0

:END
popd
