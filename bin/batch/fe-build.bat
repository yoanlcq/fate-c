@echo off
setlocal EnableDelayedExpansion
setlocal EnableExtensions

set oldwd=""
:until_fatefile
if not exist fatefile.mk (
    if "%oldwd%"=="%cd%" echo Could not find fatefile.mk^^! && exit /b 1
    set oldwd=%cd%
    cd ..
    goto until_fatefile
)

mingw32-make -f %FE_PATH%/Makefile %*
