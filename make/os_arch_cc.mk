define OS_ARCH_CC_ERROR


OS, ARCH and CC are not defined. 
Please set them on the command line when calling make,
each with one of the following values :

OS = windows | linux | osx
ARCH = 32 | 64 (don't specify for osx)
CC = gcc | cl

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

The CC variable must be set to either gcc or cl.

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
#Empty for OS X
ARCH=
endif

