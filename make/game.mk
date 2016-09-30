$(GAME)_CFILES = $(wildcard src/$(GAME)/*.c \
							src/$(GAME)/*/*.c \
							src/$(GAME)/*/*/*.c \
							src/$(GAME)/*/*/*/*.c \
							src/$(GAME)/*/*/*/*/*.c)
$(GAME)_OFILES = $(patsubst src/$(GAME)/%.c,$(BUILDDIR)/$(GAME)/%$(OBJ_EXTENSION),$($(GAME)_CFILES))
$(GAME)_DBG_OFILES = $(patsubst src/$(GAME)/%.c,$(BUILDDIR)/$(GAME)_dbg/%$(OBJ_EXTENSION),$($(GAME)_CFILES))


$(BUILDDIR)/$(GAME)/%$(OBJ_EXTENSION): src/$(GAME)/%.c | dirs
	$(call MKDIR_P,$(@D))
	@echo " ==> $@"
	$(SEE_OBJ_CMD)$(CC) $(CCRELEASEFLAGS) $(if $(CAI_ENABLE),$(if $(filter $(CAI_EXCLUDE),$<),,$(CAI_CCFLAGS))) $(CCOBJ) $< $(CCOUT_OBJ)$@
$(BUILDDIR)/$(GAME)_dbg/%$(OBJ_EXTENSION): src/$(GAME)/%.c | dirs
	@$(call MKDIR_P,$(@D))
	@echo " ==> $@"
	$(SEE_OBJ_CMD)$(CC) $(CCDEBUGFLAGS) $(if $(CAI_ENABLE),$(if $(filter $(CAI_EXCLUDE),$<),,$(CAI_CCFLAGS))) $(CCOBJ) $< $(CCOUT_OBJ)$@

ifeq ($(OS),windows)
ifeq ($(CC),cl)
$(BUILDDIR)/$(GAME)/$(GAME).res:src/$(GAME)/$(GAME).rc src/$(GAME)/$(GAME).ico \
						  | dirs
	@$(call MKDIR_P,$(@D))
	$(SEE_MISC_CMD)rc /nologo /fo $(subst /,\\,$@) $(subst /,\\,$<)
else
$(BUILDDIR)/$(GAME)/$(GAME).res:src/$(GAME)/$(GAME).rc src/$(GAME)/$(GAME).ico \
						  | dirs
	@$(call MKDIR_P,$(@D))
	$(SEE_MISC_CMD)windres $< -O coff -F $(WINDRES_TARGET) -o $@
endif
$(GAME)_OFILES     += $(BUILDDIR)/$(GAME)/$(GAME).res
$(GAME)_DBG_OFILES += $(BUILDDIR)/$(GAME)/$(GAME).res
endif

$(GAME)     = $(BINDIR)/$(GAME)$(EXE_EXTENSION)
$(GAME)_DBG = $(BINDIR)/$(GAME)_dbg$(EXE_EXTENSION)

$($(GAME)): $(FATE_OFILES) $($(GAME)_OFILES)
	@echo " ==> $@"
	$(SEE_EXE_CMD)$(CC) $(CCRELEASEFLAGS) $^ $(CCOUT_EXE)$@ $(LDLIBS)
$($(GAME)_DBG): $(FATE_DBG_OFILES) $($(GAME)_DBG_OFILES)
	@echo " ==> $@"
	$(SEE_EXE_CMD)$(CC) $(CCDEBUGFLAGS) $^ $(CCOUT_EXE)$@ $(LDLIBS)
	$(MOVEPDB)

GAMES += $($(GAME)) $($(GAME)_DBG)

