CCFLAGS = -std=c11 -Iinclude -Iinclude/contrib -Wall -D_GNU_SOURCE
ifneq ($(ARCH),)
CCFLAGS += -m$(ARCH) 
endif
CCDEBUGFLAGS = $(CCFLAGS) -g -DFATE_DEBUG_BUILD -DFATE_ENABLE_TRACING
CCRELEASEFLAGS = $(CCFLAGS) -O3 -DNDEBUG
GLEWFLAGS = -O0 -DGLEW_STATIC -DGLEW_NO_GLU
CCOBJ = -c
CCOUT_OBJ = -o 
CCOUT_EXE = -o 

ifeq ($(CC),gcc) #Can happen if we're coming from make/clang.mk.
ifeq ($(OS),linux)
CCDEBUGFLAGS += -Og -rdynamic
endif
endif

ifeq ($(OS),osx)
CCFLAGS += -F/Library/Frameworks 
LDLIBS = -framework SDL2 -framework OpenGL -lm -lintl
else
LDLIBS = -lSDL2 -l$(LIBGL) -lm
endif

ifeq ($(OS),windows)
CCRELEASEFLAGS += -mwindows
#LDLIBS += -ld3d10 -ldxguid 
LDLIBS += -lGdi32 -lUser32 -lKernel32 -lDbgHelp -lws2_32
else
endif

