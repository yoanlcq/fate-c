CCFLAGS = -Iinclude -Wall -D_GNU_SOURCE
CCDEBUGFLAGS = $(CCFLAGS) -g \
			   -DFATE_DEBUG_BUILD -DFATE_ENABLE_TRACING
GLEWFLAGS = -O0 -DGLEW_STATIC -DGLEW_NO_GLU
ifeq ($(OS),linux)
CCDEBUGFLAGS += -O0 -Wl,--export-dynamic
endif
CCRELEASEFLAGS = $(CCFLAGS) -O3 -DNDEBUG
ifneq ($(ARCH),)
CCFLAGS += -m$(ARCH) 
endif
ifneq ($(OS),osx)
LDLIBS = -lm \
		 -lSDL2
else
CCFLAGS += -F/Library/Frameworks 
LDLIBS = -framework SDL2 \
		-framework OpenGL
endif

ifeq ($(OS),windows)
CCRELEASEFLAGS += -mwindows
#LDLIBS += -ld3d10 -ldxguid 
LDLIBS += -lopengl32 -lGdi32 -lUser32 -lKernel32 -lDbgHelp
else
CCFLAGS += -std=c11 #Under MINGW, it causes a "multiple definition" error with NtCurrentTeb, getCurrentFiber and getFiberData.
ifeq ($(OS),linux)
LDLIBS += -lGL
endif
endif

# Too bad $< and $@ do not expand within macros.
define MKOBJ

$(BUILDDIR)/$(3).o : src/$(3) \
					 $(if $(4),include/$(4),) \
					 $(if $(5),include/$(5),) \
					 $(if $(6),include/$(6),) \
					 $(if $(7),include/$(7),) \
					 $(if $(8),include/$(8),)
	$(CC) $(CCRELEASEFLAGS) $(2) -c src/$(3) -o $(BUILDDIR)/$(3).o

$(BUILDDIR)/$(3)_debug.o : src/$(3) \
					       $(if $(4),include/$(4),) \
					       $(if $(5),include/$(5),) \
					       $(if $(6),include/$(6),) \
					       $(if $(7),include/$(7),) \
					       $(if $(8),include/$(8),)
	$(CC) $(CCDEBUGFLAGS) $(2) -c src/$(3) -o $(BUILDDIR)/$(3)_debug.o

$(1)_OFILES += $(BUILDDIR)/$(3).o 
$(1)_DEBUG_OFILES += $(BUILDDIR)/$(3)_debug.o 

endef

ifeq ($(OS),windows)
ifeq ($(ARCH),32)
WINDRES_TARGET=pe-i386
else
WINDRES_TARGET=pe-x86-64
endif
EXE_EXTENSION =.exe
else
EXE_EXTENSION = 
endif

define MKRES

ifeq ($(OS),windows)

$(BUILDDIR)/$(1).res: res/windres/$(1).rc res/windres/$(1).ico
	windres res/windres/$(1).rc -O coff -F $(WINDRES_TARGET) -o $(BUILDDIR)/$(1).res
$(1)_OFILES += $(BUILDDIR)/$(1).res
$(1)_DEBUG_OFILES += $(BUILDDIR)/$(1).res

endif

endef

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


