define OS_ARCH_ERROR


OS and ARCH are not defined. 
Please set them on the command line when calling make,
with one of the following values :

OS = windows | linux | osx
ARCH = 32 | 64 (don't specify for osx)

Like so : 
    make OS=linux ARCH=64


endef

ifndef OS
$(error $(call OS_ARCH_ERROR))
endif

ifneq ($(OS),osx)
ifndef ARCH
$(error $(call OS_ARCH_ERROR))
endif
endif

ifndef ARCH
#None for OS X
ARCH=
endif

.PHONY: all clean mrproper re

### COMPILER

CC = gcc
CCFLAGS = -Iinclude -O3
ifneq ($(ARCH),)
CCFLAGS += -m$(ARCH) 
endif
LDLIBS = -lm \
		 -lcsfml-audio \
		 -lcsfml-graphics \
		 -lcsfml-network \
		 -lcsfml-window \
		 -lcsfml-system 

### PROJECT FILES

BINDIR = bin/$(OS)$(ARCH)
BUILDDIR = build/$(OS)$(ARCH)
ifeq ($(OS),windows)
#Because windows' shell can't do it if there's a '/'.
BINDIR = bin\$(OS)$(ARCH)
BUILDDIR = build\$(OS)$(ARCH)
endif
EXE := $(BINDIR)/game
OFILES = 


#### SHELL

MKDIR = mkdir
ifneq ($(OS),windows)
MKDIR += -p
endif
ifeq ($(OS),linux)
CLEANCMD = rm -f $(BUILDDIR)/*
LDLIBS += -lGL
endif
ifeq ($(OS),windows)
CLEANCMD = del /f /q $(BUILDDIR)\*
LDLIBS += -lopengl32 #-lglut -lglu32 -lglew32mx -lopengl32
EXE := $(EXE).exe #To prevent useless recompilation
endif




### GOALS

all: $(BINDIR) $(BUILDDIR) $(DATADIR) $(EXE)

$(BINDIR):
	$(MKDIR) $(BINDIR)

$(BUILDDIR):
	$(MKDIR) $(BUILDDIR)

$(DATADIR):
	$(MKDIR) $(DATADIR)

define OFILE_MACRO
OFILES += $(BUILDDIR)/$(1).o 
$(BUILDDIR)/$(1).o : src/$(1).c $(2) $(3) $(4) $(5) $(6) $(7) $(8)
	$(CC) $(CCFLAGS) -c src/$(1).c -o $(BUILDDIR)/$(1).o
endef

$(eval $(call OFILE_MACRO,glew,include/GL/glew.h,include/GL/glxew.h,include/GL/wglew.h))
$(eval $(call OFILE_MACRO,display_resolutions,include/utils/display_resolutions.h))
$(eval $(call OFILE_MACRO,pathto,include/utils/pathto.h))
$(eval $(call OFILE_MACRO,opengl_debug,include/opengl_debug.h))
$(eval $(call OFILE_MACRO,glmake,include/glmake.h))
$(eval $(call OFILE_MACRO,cube,include/cube.h))
$(eval $(call OFILE_MACRO,oldmain))

#ifeq ($(OS),windows)
#$(BUILDDIR)/fst.res : src/fst/fst.rc src/fst/fst.ico
#	windres src/fst/fst.rc -O coff -o $(BUILDDIR)/fst.res
#OFILES += $(BUILDDIR)/fst.res
#endif

$(EXE) : $(OFILES)
	$(CC) $(CCFLAGS) -o $(EXE) $(OFILES) $(LDLIBS)


### PHONY GOALS

clean:
	$(CLEANCMD)
mrproper : clean all
re : mrproper
