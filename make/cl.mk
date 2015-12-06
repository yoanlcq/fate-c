CCFLAGS = /nologo /Iinclude /DGLEW_STATIC /DGLEW_NO_GLU 
CCDEBUGFLAGS = $(CCFLAGS) /Ob2 /Zi /DEBUG /DFATE_DEBUG_BUILD /DFATE_ENABLE_TRACING
CCRELEASEFLAGS = $(CCFLAGS) /O2 /Ot /Ox /GL /Gw /DNDEBUG
LDLIBS = csfml-audio-2.lib \
		 csfml-graphics-2.lib \
		 csfml-network-2.lib \
		 csfml-window-2.lib \
		 csfml-system-2.lib \
		 opengl32.lib \
		 Kernel32.lib \
		 DbgHelp.lib

# Dummy rule for include/, because when calling MKOBJ below, 
# there might not always be 8 arguments.
include/:
	

# Too bad $< and $@ do not expand within macros.
define MKOBJ

$(BUILDDIR)/$(2).obj : src/$(2) \
	                   $(if $(3),include/$(3),) \
					   $(if $(4),include/$(4),) \
					   $(if $(5),include/$(5),) \
					   $(if $(6),include/$(6),) \
					   $(if $(7),include/$(7),) \
					   $(if $(8),include/$(8),)
	$(CC) $(CCRELEASEFLAGS) /c src/$(2) /Fo$(BUILDDIR)/$(2).obj

$(BUILDDIR)/$(2)_debug.obj : src/$(2) \
                             $(if $(3),include/$(3),) \
					         $(if $(4),include/$(4),) \
					         $(if $(5),include/$(5),) \
					         $(if $(6),include/$(6),) \
					         $(if $(7),include/$(7),) \
					         $(if $(8),include/$(8),)
	$(CC) $(CCDEBUGFLAGS) -c src/$(2) /Fo$(BUILDDIR)/$(2)_debug.obj

$(1)_OBJFILES += $(BUILDDIR)/$(2).obj
$(1)_DEBUG_OBJFILES += $(BUILDDIR)/$(2)_debug.obj

endef

define MKEXE

$(BINDIR)/$(1).exe : $($(1)_OBJFILES)
	$(CC) $(CCRELEASEFLAGS) $($(1)_OBJFILES) \
		/Fe$(BINDIR)/$(1).exe $(LDLIBS)

$(BINDIR)/$(1)_debug.exe : $($(1)_DEBUG_OBJFILES)
	$(CC) $(CCDEBUGFLAGS) $($(1)_DEBUG_OBJFILES) \
		/Fe$(BINDIR)/$(1)_debug.exe $(LDLIBS) \
		&& move *.pdb $(BINDIR)

$(1): $(BINDIR)/$(1).exe
$(1)_debug: $(BINDIR)/$(1)_debug.exe

endef


