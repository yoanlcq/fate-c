CCFLAGS = -idirafter include -idirafter include/contrib -Wall
CCDEBUGFLAGS = $(CCFLAGS) -O0 -g \
			   -DFATE_DEBUG_BUILD -DFATE_ENABLE_TRACING
GLEWFLAGS = -O0 -DGLEW_STATIC -DGLEW_NO_GLU
CCRELEASEFLAGS = $(CCFLAGS) -O3 -DNDEBUG
CCFLAGS += -s USE_SDL=2

# Too bad $< and $@ do not expand within macros.
define MKOBJ

$(BUILDDIR)/$(3).js : src/$(3) \
					 $(if $(4),include/$(4),) \
					 $(if $(5),include/$(5),) \
					 $(if $(6),include/$(6),) \
					 $(if $(7),include/$(7),) \
					 $(if $(8),include/$(8),)
	$(CC) $(CCRELEASEFLAGS) $(2) -c src/$(3) -o $(BUILDDIR)/$(3).js

$(BUILDDIR)/$(3)_debug.js : src/$(3) \
					       $(if $(4),include/$(4),) \
					       $(if $(5),include/$(5),) \
					       $(if $(6),include/$(6),) \
					       $(if $(7),include/$(7),) \
					       $(if $(8),include/$(8),)
	$(CC) $(CCDEBUGFLAGS) $(2) -c src/$(3) -o $(BUILDDIR)/$(3)_debug.js

$(1)_OFILES += $(BUILDDIR)/$(3).js
$(1)_DEBUG_OFILES += $(BUILDDIR)/$(3)_debug.js

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

