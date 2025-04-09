@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

:: Compiler settings
SET CC=clang
SET CPPFLAGS=-Iinc -DBJ_CONFIG_ALL
SET CFLAGS=-Wall -Wextra -std=c99 -D_CRT_SECURE_NO_WARNINGS
SET CFLAGS_EXTRA=
SET LIB_CPPFLAGS=
SET LIB_CFLAGS=
SET EXE_CPPFLAGS=-DBANJO_ASSETS_DIR=\"$(ASSETS_DIR)\"
SET EXE_CFLAGS=

:: Validate the LIB parameter
IF /I "%~1"=="static" (
    SET LIB=static
    SET CPPFLAGS=!CPPFLAGS! -DBANJO_STATIC
) ELSE IF /I "%~1"=="shared" (
    SET LIB=shared
    SET LIB_CPPFLAGS=!LIB_CPPFLAGS! -DBANJO_EXPORTS
) ELSE (
    echo Usage: %~nx0 "[static|shared] [release|debug]"
    exit /b 1
)
SHIFT

:: Validate the BUILD parameter
IF /I "%~1"=="release" (
    SET BUILD=release
    SET CFLAGS=!CFLAGS! -O2
) ELSE IF /I "%~1"=="debug" (
    SET BUILD=debug
    SET CFLAGS=!CFLAGS! -g
) ELSE (
    echo Usage: %~nx0 "[static|shared] [release|debug]"
    exit /b 1
)
SHIFT

SET OUTDIR=build-%LIB%-%BUILD%

@REM :: Display the output directory
@REM echo OUTDIR: !OUTDIR!
@REM echo CC: !CC!
@REM echo CPPFLAGS: !CPPFLAGS!
@REM echo CFLAGS: !CFLAGS!
@REM echo CFLAGS_EXTRA: !CFLAGS_EXTRA!
@REM echo LIB_CPPFLAGS: !LIB_CPPFLAGS!
@REM echo LIB_CFLAGS: !LIB_CFLAGS!
@REM echo EXE_CPPFLAGS: !EXE_CPPFLAGS!
@REM echo EXE_CFLAGS: !EXE_CFLAGS!

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

SHIFT
SET TARGET=%~1
GOTO make_target

:make_banjo
echo "Building Banjo..."

SET OBJS=
MKDIR !OUTDIR!\src
FOR %%f in (src\*.c) do (
    SET OBJ=!OUTDIR!\src\%%~nf.o
    SET OBJS=!OBJS! !OBJ!
    @REM !CC! %CPPFLAGS% %LIB_CPPFLAGS% %CFLAGS% %LIB_CFLAGS% %CFLAGS_EXTRA% -c -o !OBJ! %%f
    echo !CC! %CPPFLAGS% %LIB_CPPFLAGS% %CFLAGS% %LIB_CFLAGS% %CFLAGS_EXTRA% -c -o !OBJ! %%f
    @REM echo Compiled %%~f
)

    @REM !CC! %CPPFLAGS% %LIB_CPPFLAGS% %CFLAGS% %LIB_CFLAGS% %CFLAGS_EXTRA% -c -o !OBJ! %%f

goto ret_banjo

:make_examples
echo "Building Examples..."
goto ret_examples

:make_tests
echo "Building Tests..."
goto ret_tests

:make_clean
RMDIR /S /Q !OUTDIR!
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


