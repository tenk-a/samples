rem @echo off
:: Run
::  cmake -G "???" -DCMAKE_TOOLCHAIN_FILE=toolchain/???-toolchain.cmake -B bld/??? .
::  cmake --build bld/???
:: corresponding to the toolchain name in the argument.
:: [toolchain name]
::    vc-x64       vc-win32    vc-x64-md  vc-win32-md
::    mingw-x64    mingw-win32
::    watcom-win32 watcom-dos32 watcom-dos16-s
::    djgpp
::    borland-win32
pushd %~dp0
cd ..

set Toolchain=%1
set GENE=
set COMPILER=
set ARCH=
set CRT=

if "%Toolchain%"=="" call :AUTO_TOOLCHAIN_CHECK
if "%Toolchain%"=="" goto ERR_TOOLCHAIN_LIST

if not exist toolchain\%Toolchain%-toolchain.cmake goto ERR_TOOLCHAIN

for /f "tokens=1,2,3 delims=-" %%a in ("%Toolchain%") do (
    set "COMPILER=%%a"
    set "ARCH=%%b"
    set "CRT=%%c"
)

if not exist thirdparty\lib\%Toolchain% call thirdparty\install_pdcurses.bat %COMPILER% %ARCH% %CRT%

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
cmake --install bld/%Toolchain%

goto END

:AUTO_TOOLCHAIN_CHECK
@if /I not "%PATH:borland=%"=="%PATH%"         set Toolchain=borland-win32
::@if /I not "%PATH:Embarcadero=%"=="%PATH%"   set Toolchain=borland-win32
::@if /I not "%PATH:dm\bin=%"=="%PATH%"        set Toolchain=dmc-win32
::@if /I not "%PATH:dmc\bin=%"=="%PATH%"       set Toolchain=dmc-win32
@if /I not "%PATH:WATCOM=%"=="%PATH%"          set Toolchain=watcom-win32
@if /I not "%PATH:mingw=%"=="%PATH%"           set Toolchain=mingw-win32
@if /I not "%PATH:msys32=%"=="%PATH%"          set Toolchain=mingw-win32
@if /I not "%PATH:msys64\clang32=%"=="%PATH%"  set Toolchain=mingw-win32
@if /I not "%PATH:msys64\mingw32=%"=="%PATH%"  set Toolchain=mingw-win32
@if /I not "%PATH:msys64\clang64=%"=="%PATH%"  set Toolchain=mingw-x64
@if /I not "%PATH:msys64\ucrt64=%"=="%PATH%"   set Toolchain=mingw-x64
@if /I not "%PATH:djgpp=%"=="%PATH%"           set Toolchain=djgpp
@if /I not "%PATH:Microsoft Visual Studio .NET 2003=%"=="%PATH%"         set Toolchain=vc-win32
@if /I not "%PATH:Microsoft Visual Studio 8=%"=="%PATH%"                 set Toolchain=vc-win32
@if /I not "%PATH:Microsoft Visual Studio 9.0=%"=="%PATH%"               set Toolchain=vc-win32
@if /I not "%PATH:Microsoft Visual Studio 10.0=%"=="%PATH%"              set Toolchain=vc-win32
@if /I not "%PATH:Microsoft Visual Studio 11.0=%"=="%PATH%"              set Toolchain=vc-win32
@if /I not "%PATH:Microsoft Visual Studio 12.0=%"=="%PATH%"              set Toolchain=vc-win32
@if /I not "%PATH:Microsoft Visual Studio 14.0=%"=="%PATH%"              set Toolchain=vc-win32
@if /I not "%PATH:Microsoft Visual Studio\2017=%"=="%PATH%"              set Toolchain=vc-win32
@if /I not "%PATH:Microsoft Visual Studio\2019=%"=="%PATH%"              set Toolchain=vc-win32
@if /I not "%PATH:Microsoft Visual Studio\2022=%"=="%PATH%"              set Toolchain=vc-win32
@if /I not "%PATH:Microsoft Visual Studio 8\VC\BIN\amd64=%"=="%PATH%"    set Toolchain=vc-x64
@if /I not "%PATH:Microsoft Visual Studio 9.0\VC\BIN\amd64=%"=="%PATH%"  set Toolchain=vc-x64
@if /I not "%PATH:Microsoft Visual Studio 10.0\VC\BIN\amd64=%"=="%PATH%" set Toolchain=vc-x64
@if /I not "%PATH:Microsoft Visual Studio 11.0\VC\BIN\amd64=%"=="%PATH%" set Toolchain=vc-x64
@if /I not "%PATH:Microsoft Visual Studio 12.0\VC\BIN\amd64=%"=="%PATH%" set Toolchain=vc-x64
@if /I not "%PATH:Microsoft Visual Studio 14.0\VC\BIN\amd64=%"=="%PATH%" set Toolchain=vc-x64
@if /I not "%PATH:\bin\HostX64\x64=%"=="%PATH%"                          set Toolchain=vc-x64
echo %Toolchain%
exit /b 0

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
