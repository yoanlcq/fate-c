CCFLAGS = -idirafter include -idirafter include/contrib -Wall
CCDEBUGFLAGS = $(CCFLAGS) -O0 -g \
			   -DFATE_DEBUG_BUILD -DFATE_ENABLE_TRACING
GLEWFLAGS = -O0 -DGLEW_STATIC -DGLEW_NO_GLU
CCRELEASEFLAGS = $(CCFLAGS) -O3 -DNDEBUG
CCFLAGS += --emrun -s USE_SDL=2 -s ASYNCIFY=1 -s ASSERTIONS=1

# Too bad $< and $@ do not expand within macros.
define MKOBJ

$(BUILDDIR)/$(3).bc : src/$(3) \
					 $(if $(4),include/$(4),) \
					 $(if $(5),include/$(5),) \
					 $(if $(6),include/$(6),) \
					 $(if $(7),include/$(7),) \
					 $(if $(8),include/$(8),)
	$(CC) $(CCRELEASEFLAGS) $(2) src/$(3) -o $(BUILDDIR)/$(3).bc

$(BUILDDIR)/$(3)_debug.bc : src/$(3) \
					       $(if $(4),include/$(4),) \
					       $(if $(5),include/$(5),) \
					       $(if $(6),include/$(6),) \
					       $(if $(7),include/$(7),) \
					       $(if $(8),include/$(8),)
	$(CC) $(CCDEBUGFLAGS) $(2) src/$(3) -o $(BUILDDIR)/$(3)_debug.bc

$(1)_OFILES += $(BUILDDIR)/$(3).bc
$(1)_DEBUG_OFILES += $(BUILDDIR)/$(3)_debug.bc

endef

EXE_EXTENSION=.html

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

