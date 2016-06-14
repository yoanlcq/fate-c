FATE_CFILES = $(wildcard src/fate/*.c \
						 src/fate/*/*.c \
						 src/fate/*/*/*.c \
						 src/fate/*/*/*/*.c \
						 src/fate/*/*/*/*/*.c)
FATE_OFILES = $(patsubst src/fate/%.c,$(BUILDDIR)/fate/%$(OBJ_EXTENSION),$(FATE_CFILES))
FATE_DBG_OFILES = $(patsubst src/fate/%.c,$(BUILDDIR)/fate_dbg/%$(OBJ_EXTENSION),$(FATE_CFILES))

$(BUILDDIR)/fate/%$(OBJ_EXTENSION): src/fate/%.c | dirs
	@$(call MKDIR_P,$(@D))
	@echo " ==> $@"
	@$(CC) $(CCRELEASEFLAGS) $(CCOBJ) $< $(CCOUT_OBJ)$@
$(BUILDDIR)/fate_dbg/%$(OBJ_EXTENSION): src/fate/%.c | dirs
	@$(call MKDIR_P,$(@D))
	@echo " ==> $@"
	@$(CC) $(CCDEBUGFLAGS) $(CCOBJ) $< $(CCOUT_OBJ)$@
