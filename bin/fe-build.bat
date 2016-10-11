@echo off
setlocal EnableDelayedExpansion
setlocal EnableExtensions

:until_fatefile
if not exist fatefile.mk (
    cd .. || echo "Could not find fatefile!" && exit /b
    goto until_fatefile
)
mingw32-make -f %FE_PATH%/Makefile
