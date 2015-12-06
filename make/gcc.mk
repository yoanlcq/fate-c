CCFLAGS = -Iinclude -DGLEW_STATIC -DGLEW_NO_GLU #-Wall -pedantic
CCDEBUGFLAGS = $(CCFLAGS) -O0 -g \
			   -D_GNU_SOURCE -DFATE_DEBUG_BUILD -DFATE_ENABLE_TRACING
ifeq ($(OS),linux)
CCDEBUGFLAGS += -rdynamic
endif
CCRELEASEFLAGS = $(CCFLAGS) -O3 -D_GNU_SOURCE -DNDEBUG
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
CCFLAGS += -mwindows
LDLIBS += -lopengl32 -ld3d10 -lGdi32 -lUser32 -lKernel32 -lDbgHelp
else
CCFLAGS += -std=c11 #Under MINGW, it causes a "multiple definition" error with NtCurrentTeb, getCurrentFiber and getFiberData.
LDLIBS += -lGL
endif

#ifeq ($(OS),windows)
#$(BUILDDIR)/game.res : res/windres/game.rc res/windres/game.ico
#	windres res/windres/game.rc -O coff -o $(BUILDDIR)/game.res
#GAME_OFILES += $(BUILDDIR)/game.res
#endif

# Too bad $< and $@ do not expand within macros.
define MKOBJ

$(BUILDDIR)/$(2).o : src/$(2) \
					 $(if $(3),include/$(3),) \
					 $(if $(4),include/$(4),) \
					 $(if $(5),include/$(5),) \
					 $(if $(6),include/$(6),) \
					 $(if $(7),include/$(7),) \
					 $(if $(8),include/$(8),)
	$(CC) $(CCRELEASEFLAGS) -c src/$(2) -o $(BUILDDIR)/$(2).o

$(BUILDDIR)/$(2)_debug.o : src/$(2) \
                           $(if $(3),include/$(3),) \
					       $(if $(4),include/$(4),) \
					       $(if $(5),include/$(5),) \
					       $(if $(6),include/$(6),) \
					       $(if $(7),include/$(7),) \
					       $(if $(8),include/$(8),)
	$(CC) $(CCDEBUGFLAGS) -c src/$(2) -o $(BUILDDIR)/$(2)_debug.o

$(1)_OFILES += $(BUILDDIR)/$(2).o 
$(1)_DEBUG_OFILES += $(BUILDDIR)/$(2)_debug.o 

endef

ifeq ($(OS),windows)
EXE_EXTENSION =.exe
else
EXE_EXTENSION = 
endif

define MKEXE

$(BINDIR)/$(1)$(EXE_EXTENSION) : $($(1)_OFILES)
	$(CC) $(CCRELEASEFLAGS) $($(1)_OFILES) \
		-o $(BINDIR)/$(1)$(EXE_EXTENSION) $(LDLIBS)

$(BINDIR)/$(1)_debug$(EXE_EXTENSION) : $($(1)_DEBUG_OFILES)
	$(CC) $(CCDEBUGFLAGS) $($(1)_DEBUG_OFILES) \
		-o $(BINDIR)/$(1)_debug$(EXE_EXTENSION) $(LDLIBS)

$(1): $(BINDIR)/$(1)$(EXE_EXTENSION)
$(1)_debug: $(BINDIR)/$(1)_debug$(EXE_EXTENSION)

endef


