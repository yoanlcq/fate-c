CCFLAGS = -std=c11 -Iinclude -Iinclude/contrib -Wall -D_GNU_SOURCE -msse -msse2 -msse4.2
ifneq ($(ARCH),)
CCFLAGS += -m$(ARCH) 
endif
# C'mon Travis
ifeq ($(OS),linux)
ifneq ($(shell cat /usr/include/linux/hw_breakpoint.h > /dev/null 2>&1; echo $$?),0)
CCFLAGS += -DFE_LINUXPERF_UNSUPPORTED
endif
endif
ifeq ($(OS),windows)
CCFLAGS += -DUNICODE -D_UNICODE
endif

GLEWFLAGS = -DGLEW_STATIC -DGLEW_NO_GLU
CCDEBUGFLAGS = $(CCFLAGS) -g -DFE_DEBUG_BUILD -DFE_ENABLE_TRACING $(GLEWFLAGS)
CCRELEASEFLAGS = $(CCFLAGS) -O3 -DNDEBUG $(GLEWFLAGS)
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
LDLIBS = -framework SDL2 -framework OpenGL -lm -lintl -ldl
else
LDLIBS = -lSDL2 -l$(LIBGL) -lm -ldl
endif

ifeq ($(OS),linux)
ifeq ($(VULKAN),1)
LDLIBS += -lvulkan
CCDEBUGFLAGS += -DFE_USE_VULKAN 
CCRELEASEFLAGS += -DFE_USE_VULKAN 
endif
endif

ifeq ($(OS),windows)
CCRELEASEFLAGS += -mwindows
#LDLIBS += -ld3d10 -ldxguid 
LDLIBS += -lGdi32 -lUser32 -lKernel32 -lDbgHelp -lws2_32 
ifeq ($(VULKAN),1)
LDLIBS += -lvulkan-1
CCDEBUGFLAGS += -DFE_USE_VULKAN 
CCRELEASEFLAGS += -DFE_USE_VULKAN 
endif
else
endif

