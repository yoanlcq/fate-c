@echo off
setlocal EnableDelayedExpansion
setlocal EnableExtensions

:until_fatefile
if not exist fatefile.mk (
    cd ..
    goto until_fatefile
)
set FE_TOOLCHAIN_BINDIR=""
%FE_TOOLCHAIN_BINDIR%mingw32-make -f %FE_PATH%/Makefile
