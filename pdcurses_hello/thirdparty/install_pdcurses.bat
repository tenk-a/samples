rem @echo off
:: If necessary, git clone PDCurses,
:: build pdcurses according to the compiler name argument,
:: and copy the files to include and lib/TARGET directory
:: under the thirdparty directory.
::  COMPILER ARCH     (Generate)
::  vc       x64    | vc-x64
::  vc       win32  | vc-win32
::  mingw    x64    | mingw-x64
::  mingw    win32  | mingw-win32
::  watcom          | watcom-win32 watcom-dos32 watcom-dos16-(s|c|m|l|h)
::  borland         | borland-win32
::  djgpp           | djgpp  (for dos32)
setlocal
pushd %~dp0

if exist PDCurses goto SKIP_GITHUB
git clone https://github.com/wmcbrine/PDCurses
:SKIP_GITHUB
pushd PDCurses
git pull
popd

call :install_include

set Compiler=
set Arch=
set BldTyp=
set Gen=
set Opt=
set CRT=
set LibPrefix=
set MAKE_CC=
set MAKE_CC_D=
set "MAKE_ARG=WIDE=Y UTF8=Y"
set "MAKE_ARG_D=%MAKE_ARG% DEBUG=Y"

:ARG_LOOP
  if "%1"=="" goto ARG_LOOP_EXIT

  if /I "%1"=="vc"       set Compiler=vc
  if /I "%1"=="mingw"    set Compiler=mingw
  if /I "%1"=="watcom"   set Compiler=watcom
  if /I "%1"=="borland"  set Compiler=borland
  if /I "%1"=="djgpp"    set Compiler=djgpp

  if /I "%1"=="x86"      set Arch=win32
  if /I "%1"=="Win32"    set Arch=win32
  if /I "%1"=="x64"      set Arch=x64
  if /I "%1"=="Win64"    set Arch=x64

  if /I "%1"=="md"       set CRT=md
  ::if /I "%1"=="MT"     set CRT=

  shift
goto ARG_LOOP
:ARG_LOOP_EXIT

if "%Compiler%"=="" goto ERR_1
if /I "%Compiler%"=="vc"      goto L_NMAKE
if /I "%Compiler%"=="mingw"   goto L_MingwMAKE
if /I "%Compiler%"=="watcom"  goto L_WMAKE
if /I "%Compiler%"=="borland" goto L_TMAKE
if /I "%Compiler%"=="djgpp"   goto L_DjgppMAKE
goto ERR_1

:L_NMAKE
if not "%ARCH%"=="" goto L_NMAKE_1
set ARCH=x64
cl.exe 2>&1 | findstr /C:"x86" >nul
if %ERRORLEVEL% neq 0 goto L_NAME_1
set ARCH=win32
:L_NMAKE_1
set make=nmake
set Makefile=Makefile.vc
set ext=lib
if /I "%CRT%"=="md" goto L_NMAKE_2
set MAKE_CC="CC=cl -nologo -MT"
set MAKE_CC_D="CC=cl -nologo -MTd"
:L_NMAKE_2
call :win_compile
goto END

:L_MingwMAKE
set Compiler=mingw
set make=make
set Makefile=Makefile
set ext=a
set LibPrefix=lib
call :win_compile
goto END

:L_DjgppMAKE
set Compiler=djgpp
set make=make
set Makefile=Makefile
set ext=a
set LibDir=%Compiler%
set LibPrefix=lib
set "WorkDir=dos"
set "MAKE_ARG="
set "MAKE_ARG_D=DEBUG_=Y"
call :compile
goto END

:L_WMAKE
set Compiler=watcom
set make=wmake
set Makefile=Makefile.wcc
set ext=lib
set Arch=win32
call :win_compile
call :dos32compile
call :dos16compile s
call :dos16compile c
call :dos16compile m
call :dos16compile l
call :dos16compile h
goto END

:L_TMAKE
set Compiler=borland
set Arch=win32
set make=tmake
set Makefile=Makefile.bcc
set ext=lib
call :win_compile
goto END

:install_include
set incdir=%CD%\include
if not exist %incdir% mkdir %incdir%
copy /b PDCurses\*.h %incdir%\
exit /b 0

:dos32compile
set LibDir=%Compiler%-dos32
set MDL=f
goto L_DOS_COMPILE

:dos16compile
set MDL=%1
set LibDir=%Compiler%-dos16-%MDL%

:L_DOS_COMPILE
set "WorkDir=dos"
set "MAKE_ARG=MODEL=%MDL%"
set "MAKE_ARG_D=%MAKE_ARG% DEBUG_=Y"
goto compile

:win_compile
set WorkDir=wincon
set LibDir=%Compiler%
if not "%Arch%"=="" set LibDir=%Compiler%-%Arch%
if not "%CRT%"==""  set LibDir=%Compiler%-%Arch%-%CRT%

:compile
set dstdir=%CD%\lib\%LibDir%
if not exist %dstdir% mkdir %dstdir%
set dstdirD=%CD%\lib\debug\%LibDir%
if not exist %dstdirD% mkdir %dstdirD%

pushd PDCurses\%WorkDir%
del *.obj *.o *.lib *.a *.pdb *.map *.ilb *.bak *.err

%make% -f %Makefile% %MAKE_ARG% %MAKE_CC%
copy /b pdcurses.%ext% %dstdir%\%LibPrefix%pdcurses.%ext%
del *.obj *.o *.lib *.a *.pdb *.map *.ilb *.bak *.err

%make% -f %Makefile% %MAKE_ARG_D% %MAKE_CC_D%
copy /b pdcurses.%ext% %dstdirD%\%LibPrefix%pdcurses.%ext%
del *.obj *.o *.lib *.a *.pdb *.map *.ilb *.bak *.err
popd
exit /b 0

:ERR_1
@echo install_pdcurses [Compiler]
@echo   Compiler vc mingw watcom borland djgpp
goto END

:END

popd
endlocal
