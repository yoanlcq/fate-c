define OS_ARCH_CC_ERROR


OS, ARCH and CC are not defined. 
Please set them on the command line when calling make,
each with one of the following values :

OS = windows | linux | osx | web
ARCH = 32 | 64 (don't specify for osx)
CC = gcc | cl | clang | emcc

Exemple : 
    make OS=linux ARCH=64 CC=gcc


endef

define OS_ERROR

The OS variable must be set to either windows, linux, osx or web.

endef

define ARCH_ERROR

The ARCH variable must be set to either 32 or 64.

endef

define CC_ERROR

The CC variable must be set to either gcc, cl, clang or emcc.

endef


ifndef OS
$(error $(call OS_ARCH_CC_ERROR))
else
ifneq ($(OS),windows)
ifneq ($(OS),linux)
ifneq ($(OS),osx)
ifneq ($(OS),web)
$(error $(call OS_ERROR))
endif
endif
endif
endif
endif

ifndef CC
$(error $(call OS_ARCH_CC_ERROR))
else
ifneq ($(CC),gcc)
ifneq ($(CC),cl)
ifneq ($(CC),clang)
ifneq ($(CC),emcc)
$(error $(call CC_ERROR))
endif
endif
endif
endif
endif


ifneq ($(OS),osx)
ifneq ($(OS),web)
ifndef ARCH
$(error $(call OS_ARCH_CC_ERROR))
else
ifneq ($(ARCH),32)
ifneq ($(ARCH),64)
$(error $(call ARCH_ERROR))
endif
endif
endif
endif
endif

ifndef ARCH
#Empty for OS X and Emscripten
ARCH=
endif

ifeq ($(ARCH),32)
WINDRES_TARGET=pe-i386
else
WINDRES_TARGET=pe-x86-64
endif

MKDIR_P = mkdir
ifeq ($(OS),windows)
	PATHSEP=\\
	EXE_EXTENSION=.exe
	DLL_PREFIX=
	DLL_EXTENSION=.dll
	LIBGL=opengl32
else
	MKDIR_P += -p
	PATHSEP=/
	EXE_EXTENSION=
	DLL_PREFIX=lib
	DLL_EXTENSION=.so
	LIBGL=GL
endif

ifeq ($(CC),cl)
	OBJ_EXTENSION=.obj
else
	OBJ_EXTENSION=.o
endif
ifeq ($(CC),emcc)
	OBJ_EXTENSION=.bc
	EXE_EXTENSION=.html
endif
MOVEPDB=

BUILDDIR = build$(PATHSEP)$(OS)$(ARCH)
BINDIR = bin$(PATHSEP)$(OS)$(ARCH)
DATADIR = data

.PHONY: dirs
dirs: | $(DATADIR) $(BINDIR) $(BUILDDIR)

$(DATADIR):
	$(MKDIR_P) $@
$(BINDIR):
	$(MKDIR_P) $@
$(BUILDDIR):
	$(MKDIR_P) $@


ifeq ($(OS),windows)
CLEANCMD = IF exist $(BUILDDIR) ( rmdir /Q /S $(BUILDDIR) && mkdir $(BUILDDIR) )
else
CLEANCMD = rm -rf $(BUILDDIR)/*
endif

.PHONY: clean mrproper re

clean: 
	$(CLEANCMD)
mrproper: clean all
re: mrproper
