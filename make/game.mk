$(GAME)_CFILES = $(notdir $(basename $(wildcard src/$(GAME)/*.c)))
$(GAME)_DBG_OFILES = $(addsuffix $(OBJ_EXTENSION), \
				  $(addprefix $(BUILDDIR)/$(GAME)_dbg/, $($(GAME)_CFILES)))
$(GAME)_OFILES     = $(addsuffix $(OBJ_EXTENSION), \
				  $(addprefix $(BUILDDIR)/$(GAME)/, $($(GAME)_CFILES)))

$(BUILDDIR)$(PATHSEP)$(GAME):
	$(MKDIR_P) $@
$(BUILDDIR)$(PATHSEP)$(GAME)_dbg:
	$(MKDIR_P) $@

$(BUILDDIR)/$(GAME)/%$(OBJ_EXTENSION): src/$(GAME)/%.c \
								   | dirs $(BUILDDIR)$(PATHSEP)$(GAME)
	$(CC) $(CCRELEASEFLAGS) $(CCOBJ) $< $(CCOUT_OBJ) $@
$(BUILDDIR)/$(GAME)_dbg/%$(OBJ_EXTENSION): src/$(GAME)/%.c \
									   | dirs $(BUILDDIR)$(PATHSEP)$(GAME)_dbg
	$(CC) $(CCDEBUGFLAGS) $(CCOBJ) $< $(CCOUT_OBJ) $@

ifeq ($(OS),windows)
ifeq ($(CC),cl)
$(BUILDDIR)\$(GAME)\$(GAME).res: src\$(GAME)\$(GAME).rc res/$(GAME)/$(GAME).ico \
						  | dirs $(BUILDDIR)$(PATHSEP)$(GAME)
	rc $< /fo $@
else
$(BUILDDIR)/$(GAME)/$(GAME).res: src/$(GAME)/$(GAME).rc res/$(GAME)/$(GAME).ico \
						  | dirs $(BUILDDIR)$(PATHSEP)$(GAME)
	windres $< -O coff -F $(WINDRES_TARGET) -o $@

endif
$(GAME)_OFILES     += $(BUILDDIR)/$(GAME)/$(GAME).res
$(GAME)_DBG_OFILES += $(BUILDDIR)/$(GAME)/$(GAME).res
endif

$(GAME) = $(BINDIR)/$(GAME)$(EXE_EXTENSION)
$($(GAME)): $($(GAME)_OFILES) $(FATE_OFILES)
	$(CC) $(CCRELEASEFLAGS) $^ $(CCOUT_EXE) $@ $(LDLIBS)
$(GAME)_DBG = $(BINDIR)/$(GAME)_dbg$(EXE_EXTENSION)
$($(GAME)_DBG): $($(GAME)_DBG_OFILES) $(FATE_DBG_OFILES)
	$(CC) $(CCDEBUGFLAGS) $^ $(CCOUT_EXE) $@ $(LDLIBS) $(MOVEPDB)

GAMES += $($(GAME)) $($(GAME)_DBG)
