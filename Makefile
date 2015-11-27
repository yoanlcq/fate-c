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
CCFLAGS = -Iinclude
CCDEBUGFLAGS = -O0 -g -rdynamic -DFATE_DEBUG_BUILD -DFATE_ENABLE_TRACING
CCRELEASEFLAGS = -O3
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
DATADIR = data
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
CLEANCMD = rm -f $(BUILDDIR)/*
LDLIBS += -lGL
else
CLEANCMD = del /f /q $(BUILDDIR)\*
LDLIBS += -lopengl32 #-lglut -lglu32 -lglew32mx
EXE := $(EXE).exe #To prevent useless recompilation
endif




### GOALS

all: $(BINDIR) $(BUILDDIR) $(DATADIR) $(DATADIR)/OpenGL $(EXE)

$(BINDIR):
	$(MKDIR) $(BINDIR)

$(BUILDDIR):
	$(MKDIR) $(BUILDDIR)

$(DATADIR):
	$(MKDIR) $(DATADIR)

$(DATADIR)/OpenGL:
	$(MKDIR) $(DATADIR)/OpenGL

#ifeq ($(OS),windows)
#$(BUILDDIR)/game.res : res/windres/game.rc res/windres/game.ico
#	windres res/windres/game.rc -O coff -o $(BUILDDIR)/game.res
#OFILES += $(BUILDDIR)/game.res
#endif

define MKOBJ
OFILES += $(BUILDDIR)/$(1).o 
$(BUILDDIR)/$(1).o : src/$(1).c include/$(2) include/$(3) include/$(4) include/$(5) include/$(6) include/$(7) include/$(8)
	$(CC) $(CCFLAGS) $(CCRELEASEFLAGS) -c src/$(1).c -o $(BUILDDIR)/$(1).o
DEBUG_OFILES += $(BUILDDIR)/$(1).debug.o 
$(BUILDDIR)/$(1).debug.o : src/$(1).c include/$(2) include/$(3) include/$(4) include/$(5) include/$(6) include/$(7) include/$(8)
	$(CC) $(CCFLAGS) $(CCDEBUGFLAGS) -c src/$(1).c -o $(BUILDDIR)/$(1).debug.o

endef

# Be careful : There's a reason why there are no spaces between commas.

$(eval $(call MKOBJ,glew,GL/glew.h,GL/glxew.h,GL/wglew.h))

$(EXE) : $(OFILES)
	$(CC) $(CCFLAGS) $(CCRELEASEFLAGS) -o $(EXE) $(OFILES) $(LDLIBS)
$(DEBUG_EXE) : $(DEBUG_OFILES)
	$(CC) $(CCFLAGS) $(CCDEBUGFLAGS) -o $(EXE) $(DEBUG_OFILES) $(LDLIBS)



### PHONY GOALS

clean:
	$(CLEANCMD)
mrproper : clean all
re : mrproper
