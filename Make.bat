@ECHO off
SETLOCAL ENABLEDELAYEDEXPANSION

REM ##########################################################################################
REM ### CHECKING PARAMETERS ##################################################################
IF /I "%~1" neq "static" if /i "%~1" neq "shared" (
    ECHO Invalid first parameter %~1
    ECHO Usage: %~nx0 "[static|shared] [release|debug] tgt0 tgt1..."
    exit /b 1
)
SET TYPE=%~1
SHIFT
IF /I "%~1" neq "release" if /i "%~1" neq "debug" (
    ECHO Invalid second parameter %~1
    ECHO Usage: %~nx0 "[static|shared] [release|debug] tgt0 tgt1..."
    exit /b 1
)
SET CONFIG=%~1
SHIFT

REM ##########################################################################################
REM ### GENERAL CONFIGURATION ################################################################
SET OUTDIR=build-!TYPE!-!CONFIG!
SET BASENAME=banjo
SET LIBNAME=!BASENAME!.lib
SET DLLNAME=!BASENAME!.dll
SET PDBNAME=!BASENAME!.pdb
SET LIBPATH=!OUTDIR!\!LIBNAME!
SET DLLPATH=!OUTDIR!\!DLLNAME!
SET PDBPATH=!OUTDIR!\!PDBNAME!
for %%F in ("assets") do SET "ASSETS_DIR=%%~fF"
SET CC=cl
SET AR=lib
SET LD=link

REM ##########################################################################################
REM ### COMPILER AND LINKER OPTIONS ##########################################################
REM ### Edit these as you like
REM ### CPPFLAGS           : Preprocessor options
REM ### CFLAGS             : Compiler options
REM ### CFLAGS_EXTRA       : Additional CFLAGS that do not impact resulting binary
REM ### ARFLAGS            : Options passed to the static linker program
REM ### ARFLAGS_EXTRA      : Additional ARFLAGS that do not impact resulting binary
REM ### LINKFLAGS          : Options passed to the linker
REM ### LINKFLAGS_EXTRA    : Additional LINKFLAGS that do not impact resulting binary
REM ###
REM ### CFLAGS, CFLAGS_EXTRA, LINKFLAGS and LINKFLAGS_EXTRA are also available with the _debug
REM ### or _release suffir (for example LINKFLAG_EXTRA_debug) to enable these options only in
REM ### debug or release configuration respectively.
SET CPPFLAGS=/Iinc /D "BJ_CONFIG_ALL" /D_CRT_SECURE_NO_WARNINGS
SET CFLAGS=/W4 /Zc:forScope /Zc:wchar_t /Zc:inline /fp:precise /GS /Gd
SET CFLAGS_EXTRA=/errorReport:prompt /nologo /diagnostics:column /Fd"!PDBPATH!"
SET CFLAGS_debug=/Od /Ob0 /Zi /RTC1 /MDd 
SET CFLAGS_release=/O2 /Ob2 /MD
SET ARFLAGS=
SET ARFLAGS_EXTRA=/NOLOGO
SET LINKFLAGS= /NXCOMPAT "kernel32.lib" "user32.lib" /TLBID:1
SET LINKFLAGS_EXTRA=/NOLOGO /ERRORREPORT:PROMPT /PDB:"!PDBPATH!"
SET LINKFLAGS_debug=/DEBUG
SET LINKFLAGS_EXTRA_debug=
SET LINKFLAGS_release=/INCREMENTAL

REM ### ADDITIONAL FLAGS SPECIFIC FOR BUILDING BANJO LIBRARY #################################
SET CPPFLAGS_BANJO=
SET CFLAGS_BANJO=
SET LINKFLAGS_BANJO=/IMPLIB:"!LIBPATH!" /MANIFEST /DLL

REM ### ADDITIONAL FLAGS SPECIFIC FOR BUILDING TESTS AND EXAMPLES ############################
SET CPPFLAGS_EXE=/DBANJO_ASSETS_DIR="\"!ASSETS_DIR!\""
SET CFLAGS_EXE=
SET LINKFLAGS_EXE=/LIBPATH:"!OUTDIR!" "!LIBNAME!" /SUBSYSTEM:CONSOLE

REM ### OTHER FLAGS ##########################################################################
IF /I "!TYPE!"=="static" SET CPPFLAGS=!CPPFLAGS! /D "BJ_STATIC"
IF /I "!TYPE!"=="shared" SET CPPFLAGS_BANJO=!CPPFLAGS_BANJO! /D "BJ_EXPORTS"
SET CPPFLAGS=!CPPFLAGS! !CPPFLAGS_%CONFIG%!
SET CFLAGS=!CFLAGS! !CFLAGS_%CONFIG%!
SET CFLAGS_EXTRA=!CFLAGS_EXTRA! !CFLAGS_EXTRA_%CONFIG%!
SET LINKFLAGS=!LINKFLAGS! !LINKFLAGS_%CONFIG%!
SET LINKFLAGS_EXTRA=!LINKFLAGS_EXTRA! !LINKFLAGS_EXTRA_%CONFIG%!

REM ### AUTOMATICALLY SETTING ALL WHEN NO TARGET IS SPECIFIED ################################
IF "%~1"=="" (SET TARGET=all) ELSE (SET TARGET=%~1)

REM ### TARGET DISPATCH ######################################################################
:make_target
IF "!TARGET!"=="" GOTO end
IF /I "!TARGET!"=="all" goto make_banjo
IF /I "!TARGET!"=="banjo" goto make_banjo
:ret_banjo
IF /I "!TARGET!"=="all" goto make_examples
IF /I "!TARGET!"=="examples" goto make_examples
:ret_examples
IF /I "!TARGET!"=="all" goto make_tests
IF /I "!TARGET!"=="test" goto make_tests
IF /I "!TARGET!"=="tests" goto make_tests
:ret_tests
IF /I "!TARGET!"=="test" goto make_test
:ret_test
IF /I "!TARGET!"=="clean" goto make_clean
:ret_clean
IF /I "!TARGET!"=="flags" goto make_flags
:ret_flags
IF /I "!TARGET!"=="linkflags" goto make_linkflags
:ret_linkflags
IF /I "!TARGET!"=="outdir" goto make_outdir
:ret_outdir
SHIFT
SET TARGET=%~1
GOTO make_target

REM ##########################################################################################
REM ### BANJO LIBRARY ########################################################################
:make_banjo
SET OBJS=
MKDIR !OUTDIR!\src
FOR %%f in (src\*.c) do (
    SET OBJ=!OUTDIR!\src\%%~nf.obj
    SET OBJS=!OBJS! !OBJ!
    !CC! !CPPFLAGS! !CPPFLAGS_BANJO! !CFLAGS! !CFLAGS_BANJO! !CFLAGS_EXTRA! /Fo"!OBJ!" /c %%~f
    IF !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
)

IF /I "!TYPE!"=="static" (
    !AR! !ARFLAGS! !ARFLAGS_BANJO! !ARFLAGS_EXTRA! /OUT:!LIBPATH! !OBJS!
    IF !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
)

IF /I "!TYPE!"=="shared" (
    !LD! !LINKFLAGS! !LINKFLAGS_BANJO! !LINKFLAGS_EXTRA! /OUT:"!DLLPATH!" !OBJS!
    IF !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
)
goto ret_banjo

REM ##########################################################################################
REM ### EXAMPLE CODES ########################################################################
:make_examples
MKDIR !OUTDIR!\examples
FOR %%f in (examples\*.c) do (
    SET OBJ=!OUTDIR!\examples\%%~nf.obj
    SET EXE=!OUTDIR!\%%~nf.exe
    !CC! !CPPFLAGS! !CPPFLAGS_EXE! !CFLAGS! !CFLAGS_EXE! !CFLAGS_EXTRA! /Fo"!OBJ!" /c %%~f
    IF !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
    !LD! !LINKFLAGS! !LINKFLAGS_EXE! !LINKFLAGS_EXTRA! /OUT:!EXE! !OBJ!
    IF !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
)
goto ret_examples

REM ##########################################################################################
REM ### TESTS ################################################################################
:make_tests
MKDIR !OUTDIR!\test
FOR %%f in (test\*.c) do (
    SET OBJ=!OUTDIR!\test\%%~nf.obj
    SET EXE=!OUTDIR!\%%~nf.exe
    !CC! !CPPFLAGS! !CPPFLAGS_EXE! !CFLAGS! !CFLAGS_EXE! !CFLAGS_EXTRA! /Fo"!OBJ!" /Isrc /c %%~f
    IF !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
    !LD! !LINKFLAGS! !LINKFLAGS_EXE! !LINKFLAGS_EXTRA! /OUT:!EXE! !OBJ!
    IF !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
)
goto ret_tests

REM ##########################################################################################
REM ### RUN TESTS ############################################################################
:make_test
FOR %%f in (test\*.c) do (
    SET EXE=!OUTDIR!\%%~nf.exe
    !EXE!
    IF !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
)
goto ret_test

REM ##########################################################################################
REM ### DISPLAY LIBRARY COMPILE FLAGS ########################################################
:make_flags
ECHO !CPPFLAGS! !CPPFLAGS_BANJO! !CFLAGS! !CFLAGS_BANJO!
goto ret_flags

REM ##########################################################################################
REM ### DISPLAY LIBRARY LINKER FLAGS #########################################################
:make_linkflags
IF /I "!TYPE!"=="static" ECHO !ARFLAGS! !ARFLAGS_BANJO! !ARFLAGS_EXTRA!
IF /I "!TYPE!"=="shared" ECHO !LINKFLAGS! !LINKFLAGS_BANJO! !LINKFLAGS_EXTRA!
goto ret_linkflags

REM ##########################################################################################
REM ### DISPLAY OUTPUT DIRECTORY #############################################################
:make_outdir
ECHO !OUTDIR!
goto ret_outdir

REM ##########################################################################################
REM ### CLEAN PROJECT (DELETE OUTPUT DIRECTORY) ##############################################
:make_clean
RMDIR /S /Q !OUTDIR!
ECHO Project cleaned
goto ret_clean

:end
ENDLOCAL
