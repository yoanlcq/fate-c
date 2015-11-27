#ifeq ($(OS),windows)
#$(BUILDDIR)/game.res : res/windres/game.rc res/windres/game.ico
#	windres res/windres/game.rc -O coff -o $(BUILDDIR)/game.res
#GAME_OFILES += $(BUILDDIR)/game.res
#endif

# Too bad $< and $@ do not expand within macros.
define MKOBJ

$(BUILDDIR)/$(2).o : src/$(2).c include/$(3) include/$(4) include/$(5) \
	                            include/$(6) include/$(7) include/$(8)
	$(CC) $(CCFLAGS) $(CCRELEASEFLAGS) -c src/$(2).c -o $(BUILDDIR)/$(2).o

$(BUILDDIR)/$(2)_debug.o : src/$(2).c include/$(3) include/$(4) include/$(5) \
	                                  include/$(6) include/$(7) include/$(8)
	$(CC) $(CCFLAGS) $(CCDEBUGFLAGS) -c src/$(2).c -o $(BUILDDIR)/$(2)_debug.o

$(1)_OFILES += $(BUILDDIR)/$(2).o 
$(1)_DEBUG_OFILES += $(BUILDDIR)/$(2)_debug.o 

endef

ifeq ($(OS),windows)
EXE_EXTENSION = .exe
else
EXE_EXTENSION = 
endif

define MKEXE

$(BINDIR)/$(1)$(EXE_EXTENSION) : $($(1)_OFILES)
	$(CC) $(CCFLAGS) $(CCRELEASEFLAGS) $($(1)_OFILES) \
		-o $(BINDIR)/$(1)$(EXE_EXTENSION) $(LDLIBS)

$(BINDIR)/$(1)_debug$(EXE_EXTENSION) : $($(1)_DEBUG_OFILES)
	$(CC) $(CCFLAGS) $(CCDEBUGFLAGS) $($(1)_DEBUG_OFILES) \
		-o $(BINDIR)/$(1)_debug$(EXE_EXTENSION) $(LDLIBS)

$(1): $(BINDIR)/$(1)$(EXE_EXTENSION)
$(1)_debug: $(BINDIR)/$(1)_debug$(EXE_EXTENSION)

endef


