FATE_CFILES := $(wildcard src/fate/*.c \
                          src/fate/*/*.c \
                          src/fate/*/*/*.c \
                          src/fate/*/*/*/*.c \
                          src/fate/*/*/*/*/*.c) \
			   $(wildcard src/contrib/*.c \
                          src/contrib/*/*.c \
                          src/contrib/*/*/*.c \
                          src/contrib/*/*/*/*.c \
                          src/contrib/*/*/*/*/*.c)

ifeq ($(CC),emcc)
FATE_CFILES := $(filter-out %/contrib/glew.c, $(FATE_CFILES))
endif
FATE_OFILES := $(patsubst src/fate/%.c,$(BUILDDIR)/fate/%$(OBJ_EXTENSION),$(FATE_CFILES))
FATE_OFILES := $(patsubst src/contrib/%.c,$(BUILDDIR)/contrib/%$(OBJ_EXTENSION),$(FATE_OFILES))
FATE_DBG_OFILES := $(patsubst src/fate/%.c,$(BUILDDIR)/fate_dbg/%$(OBJ_EXTENSION),$(FATE_CFILES))
FATE_DBG_OFILES := $(patsubst src/contrib/%.c,$(BUILDDIR)/contrib_dbg/%$(OBJ_EXTENSION),$(FATE_DBG_OFILES))


$(BUILDDIR)/contrib/%$(OBJ_EXTENSION): src/contrib/%.c | dirs
	@$(call MKDIR_P,$(@D))
	@echo " ==> $@"
	@$(CC) $(CCRELEASEFLAGS) $(CCOBJ) $< $(CCOUT_OBJ)$@

$(BUILDDIR)/contrib_dbg/%$(OBJ_EXTENSION): src/contrib/%.c | dirs
	@$(call MKDIR_P,$(@D))
	@echo " ==> $@"
	@$(CC) $(CCDEBUGFLAGS) $(CCOBJ) $< $(CCOUT_OBJ)$@

$(BUILDDIR)/fate/%$(OBJ_EXTENSION): src/fate/%.c | dirs
	@$(call MKDIR_P,$(@D))
	@echo " ==> $@"
	@$(CC) $(CCRELEASEFLAGS) $(CCOBJ) $< $(CCOUT_OBJ)$@

$(BUILDDIR)/fate_dbg/%$(OBJ_EXTENSION): src/fate/%.c | dirs
	@$(call MKDIR_P,$(@D))
	@echo " ==> $@"
	$(CC) $(CCDEBUGFLAGS) $(CCOBJ) $< $(CCOUT_OBJ)$@
