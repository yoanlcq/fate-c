FATE_CFILES     = $(notdir $(basename $(wildcard src/fate/*.c)))
FATE_OFILES     = $(addsuffix $(OBJ_EXTENSION), \
				  $(addprefix $(BUILDDIR)/fate/, $(FATE_CFILES)))
FATE_DBG_OFILES = $(addsuffix $(OBJ_EXTENSION), \
				  $(addprefix $(BUILDDIR)/fate_dbg/, $(FATE_CFILES)))

$(BUILDDIR)$(PATHSEP)fate:
	$(MKDIR_P) $@
$(BUILDDIR)$(PATHSEP)fate_dbg:
	$(MKDIR_P) $@

$(BUILDDIR)/fate/%$(OBJ_EXTENSION): src/fate/%.c \
								   | $(BUILDDIR)$(PATHSEP)fate dirs
	$(CC) $(CCRELEASEFLAGS) $(CCOBJ) $< $(CCOUT_OBJ) $@
$(BUILDDIR)/fate_dbg/%$(OBJ_EXTENSION): src/fate/%.c \
									   | $(BUILDDIR)$(PATHSEP)fate_dbg dirs
	$(CC) $(CCDEBUGLAGS) $(CCOBJ) $< $(CCOUT_OBJ) $@
