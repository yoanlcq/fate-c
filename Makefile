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

ifeq ($(OS),windows)
BUILDDIR = build\$(OS)$(ARCH)
else
BUILDDIR = build/$(OS)$(ARCH)
endif
BINDIR = bin/$(OS)$(ARCH)
DATADIR = data

#### SHELL

MKDIR = mkdir
ifneq ($(OS),windows)
MKDIR += -p
CLEANCMD = rm -f $(BUILDDIR)/*
LDLIBS += -lGL
else
CLEANCMD = del /f /q $(BUILDDIR)\*
LDLIBS += -lopengl32 #-lglut -lglu32 -lglew32mx
GAME_EXE := $(GAME_EXE).exe #To prevent useless recompilation
endif



### GOALS

.PHONY: all directories default_goals clean mrproper re

all: directories default_goals
directories: $(BINDIR) $(BUILDDIR) $(DATADIR) $(DATADIR)/OpenGL

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
#GAME_OFILES += $(BUILDDIR)/game.res
#endif

define MKOBJ

$(BUILDDIR)/$(2).o : src/$(2).c include/$(3) include/$(4) include/$(5) \
	                            include/$(6) include/$(7) include/$(8)
	$(CC) $(CCFLAGS) $(CCRELEASEFLAGS) -c src/$(2).c -o $(BUILDDIR)/$(2).o

$(BUILDDIR)/$(2).debug.o : src/$(2).c include/$(3) include/$(4) include/$(5) \
	                                  include/$(6) include/$(7) include/$(8)
	$(CC) $(CCFLAGS) $(CCDEBUGFLAGS) -c src/$(2).c -o $(BUILDDIR)/$(2).debug.o

$(1)_OFILES += $(BUILDDIR)/$(2).o 
DEBUG_$(1)_OFILES += $(BUILDDIR)/$(2).debug.o 

endef

define MKEXE
$(1) : $(2)
	$(CC) $(CCFLAGS) $(3) $(2) -o $(1) $(LDLIBS)
endef


# Be careful : There's a reason why there are no spaces between commas.

$(eval $(call MKOBJ,GAME,glew,GL/glew.h,GL/glxew.h,GL/wglew.h))
$(eval $(call MKEXE,$(BINDIR)/game,$(GAME_OFILES),$(CCRELEASEFLAGS)))
$(eval $(call MKEXE,$(BINDIR)/game_debug,$(DEBUG_GAME_OFILES),$(CCDEBUGFLAGS)))
game : $(BINDIR)/game
game_debug : $(BINDIR)/game_debug

$(eval $(call MKOBJ,CUBE,cube))
$(eval $(call MKEXE,$(BINDIR)/cube,$(CUBE_OFILES),$(CCRELEASEFLAGS)))
$(eval $(call MKEXE,$(BINDIR)/cube_debug,$(DEBUG_CUBE_OFILES),$(CCDEBUGFLAGS)))
cube : $(BINDIR)/cube
cube_debug : $(BINDIR)/cube_debug

default_goals: cube_debug cube

### PHONY GOALS

clean:
	$(CLEANCMD)
mrproper : clean all
re : mrproper
