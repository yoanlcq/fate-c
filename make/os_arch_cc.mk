define OS_ARCH_CC_ERROR


OS, ARCH and CC are not defined. 
Please set them on the command line when calling make,
each with one of the following values :

OS = windows | linux | osx
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
$(error $(call OS_ERROR))
endif
endif
endif
endif

ifndef CC
$(error $(call OS_ARCH_CC_ERROR))
else
ifneq ($(CC),gcc)
ifneq ($(CC),clang)
ifneq ($(CC),cl)
ifneq ($(CC),emcc)
$(error $(call CC_ERROR))
endif
endif
endif
endif
endif


ifneq ($(OS),osx)
ifneq ($(CC),emcc)
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

ifeq ($(OS),windows)
	MKDIR_P = if not exist $(subst /,\\,$(1)) ( mkdir $(subst /,\\,$(1)) )
	PATHSEP=\\
	EXE_EXTENSION=.exe
	DLL_PREFIX=
	DLL_EXTENSION=.dll
	LIBGL=opengl32
else
	MKDIR_P = mkdir -p $(1)
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

OSARCH=$(OS)$(ARCH)
ifeq ($(CC),emcc)
	OBJ_EXTENSION=.bc
	EXE_EXTENSION=.html
	OSARCH=web
endif
MOVEPDB=

%/:
	$(call MKDIR_P,$@)

BUILDDIR = build/$(OSARCH)
BINDIR = bin/$(OSARCH)
DATADIR = data

.PHONY: dirs
dirs: | $(DATADIR)\/ $(BINDIR)\/ $(BUILDDIR)\/

ifeq ($(OS),windows)
CLEANCMD = if exist $(subst /,\\,$(BUILDDIR)) ( rmdir /Q /S $(subst /,\\,$(BUILDDIR)) )
else
CLEANCMD = rm -rf $(BUILDDIR)
endif

.PHONY: clean mrproper re

clean: 
	$(CLEANCMD)
mrproper: clean all
re: mrproper
