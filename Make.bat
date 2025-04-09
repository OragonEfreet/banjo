@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

:: Validate parameters
IF /I "%~1" neq "static" if /i "%~1" neq "shared" (
    echo Usage: %~nx0 "[static|shared] [release|debug]"
    exit /b 1
)
set LIB=%~1
SHIFT

:: Validate parameters
IF /I "%~1" neq "release" if /i "%~1" neq "debug" (
    echo Usage: %~nx0 "[static|shared] [release|debug]"
    exit /b 1
)
set BUILD=%~1
SHIFT

SET OUTDIR=build-!LIB!-!BUILD!
SET BASENAME=libbanjo


:: Compiler settings
SET CC=cl


@REM Common flags set for all targets
@REM It is also possible to provide per BUILD CPPFLAGS, CFLAGS and CFLAGS_EXTRA by appending _release or _debug
@REM _EXTRA flags are passed while compiling like any other flags but they don't appear when calling the 'flags' command.
SET CPPFLAGS=/Iinc /D "BJ_CONFIG_ALL"
SET CFLAGS=/W4 /Zc:forScope /Zc:wchar_t /Zc:inline /fp:precise /GS /Gd
SET CFLAGS_EXTRA=/errorReport:prompt /nologo /diagnostics:column

SET CFLAGS_debug=/Od /Ob0 /Zi /RTC1 /MDd 
SET CFLAGS_EXTRA_debug=/Fd"!OUTDIR!\!BASENAME!.pdb"

@REM Flags specific to building Banjo
SET CPPFLAGS_BANJO=
SET CFLAGS_BANJO=

@REM flags specific to targets building against Banjo (tests and examples)
SET CPPFLAGS_EXE=
SET CFLAGS_EXE=

IF /I "!LIB!"=="static" SET CPPFLAGS=!CPPFLAGS! /D "BJ_STATIC"
IF /I "!LIB!"=="shared" SET CPPFLAGS_BANJO=!CPPFLAGS_BANJO! /D "BJ_EXPORTS"
SET CPPFLAGS=!CPPFLAGS! !CPPFLAGS_%BUILD%!
SET CFLAGS=!CFLAGS! !CFLAGS_%BUILD%!
SET CFLAGS_EXTRA=!CFLAGS_EXTRA! !CFLAGS_EXTRA_%BUILD%!

@REM TODO
@REM SET CPPFLAGS_EXE=-DBANJO_ASSETS_DIR=\"$(ASSETS_DIR)\"
@REM SET CFLAGS=-Wall -Wextra -std=c99 -D_CRT_SECURE_NO_WARNINGS


@REM :: Display the output directory
@REM echo OUTDIR: !OUTDIR!
@REM echo CC: !CC!
@REM echo CPPFLAGS: !CPPFLAGS!
@REM echo CFLAGS: !CFLAGS!
@REM echo CFLAGS_EXTRA: !CFLAGS_EXTRA!
@REM echo CPPFLAGS_BANJO: !CPPFLAGS_BANJO!
@REM echo CFLAGS_BANJO: !CFLAGS_BANJO!
@REM echo CPPFLAGS_EXE: !CPPFLAGS_EXE!
@REM echo CFLAGS_EXE: !CFLAGS_EXE!

IF "%~1"=="" (SET TARGET=all) ELSE (SET TARGET=%~1)


:make_target

IF "!TARGET!"=="" GOTO end

IF /I "!TARGET!"=="all" goto make_banjo
IF /I "!TARGET!"=="banjo" goto make_banjo
:ret_banjo

IF /I "!TARGET!"=="all" goto make_examples
IF /I "!TARGET!"=="examples" goto make_examples
:ret_examples

IF /I "!TARGET!"=="all" goto make_tests
IF /I "!TARGET!"=="tests" goto make_tests
:ret_tests

IF /I "!TARGET!"=="clean" goto make_clean
:ret_clean

IF /I "!TARGET!"=="flags" goto make_flags
:ret_flags

SHIFT
SET TARGET=%~1
GOTO make_target

:make_banjo
echo "Building Banjo..."

SET OBJS=
MKDIR !OUTDIR!\src
FOR %%f in (src\*.c) do (
    SET OBJ=!OUTDIR!\src\%%~nf.obj
    SET OBJS=!OBJS! !OBJ!
    SET COMMAND_LINE=!CPPFLAGS! !CPPFLAGS_BANJO! !CFLAGS! !CFLAGS_BANJO! !CFLAGS_EXTRA! /Fo"!OBJ!"
    !CC! !COMMAND_LINE! /c %%~f
    IF !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
)

echo "All good"

goto ret_banjo

:make_examples
echo "Building Examples..."
goto ret_examples

:make_tests
echo "Building Tests..."
goto ret_tests

:make_flags
echo !CPPFLAGS! !CPPFLAGS_BANJO! !CFLAGS! !CFLAGS_BANJO!
goto ret_flags

:make_clean
RMDIR /S /Q !OUTDIR!
echo Project Cleaned
goto ret_clean

:end
ENDLOCAL

:: Les targets que je veux build sont:
:: all: correspond à banjo, examples et tests
:: banjo: builde la lib
:: examples: builde les examples
:: tests: build les tests
:: clean: nettoie le projet
:: flags: affiche les options de compile
:: outdir: affiche le dossier de sortie
:: test: exécute les tests


