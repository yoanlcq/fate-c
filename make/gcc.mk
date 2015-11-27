CC = gcc
CCFLAGS = -Iinclude
CCDEBUGFLAGS = -O0 -g -rdynamic -DFATE_DEBUG_BUILD -DFATE_ENABLE_TRACING
CCRELEASEFLAGS = -O3 -DNDEBUG
ifneq ($(ARCH),)
CCFLAGS += -m$(ARCH) 
endif
LDLIBS = -lm \
		 -lcsfml-audio \
		 -lcsfml-graphics \
		 -lcsfml-network \
		 -lcsfml-window \
		 -lcsfml-system 

ifeq ($(OS),windows)
LDLIBS += -lopengl32 #-lglut -lglu32 -lglew32mx
else
LDLIBS += -lGL
endif


