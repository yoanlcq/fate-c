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


ifeq ($(CC),cl)
ifeq ($(ARCH),64)
FATE_ASMFILES := $(wildcard src/fate/cai/*$(ASM_EXTENSION))
FATE_ASM_OFILES := $(patsubst src/fate/%$(ASM_EXTENSION),$(BUILDDIR)/fate_asm/%$(OBJ_EXTENSION),$(FATE_ASMFILES))
FATE_OFILES     += $(FATE_ASM_OFILES)
FATE_DBG_OFILES += $(FATE_ASM_OFILES)
endif
endif


$(BUILDDIR)/fate_asm/%$(OBJ_EXTENSION): src/fate/%$(ASM_EXTENSION) | dirs
	@$(call MKDIR_P,$(@D))
	@echo " ==> $@"
	$(SEE_OBJ_CMD)$(AS) $(CCOUT_OBJ)$@ $(CCOBJ) $<

$(BUILDDIR)/contrib/%$(OBJ_EXTENSION): src/contrib/%.c | dirs
	@$(call MKDIR_P,$(@D))
	@echo " ==> $@"
	$(SEE_OBJ_CMD)$(CC) $(CCRELEASEFLAGS) $(if $(CAI_ENABLE),$(if $(filter $(CAI_EXCLUDE),$<),,$(CAI_CCFLAGS))) $(CCOBJ) $< $(CCOUT_OBJ)$@

$(BUILDDIR)/contrib_dbg/%$(OBJ_EXTENSION): src/contrib/%.c | dirs
	@$(call MKDIR_P,$(@D))
	@echo " ==> $@"
	$(SEE_OBJ_CMD)$(CC) $(CCDEBUGFLAGS) $(if $(CAI_ENABLE),$(if $(filter $(CAI_EXCLUDE),$<),,$(CAI_CCFLAGS))) $(CCOBJ) $< $(CCOUT_OBJ)$@

$(BUILDDIR)/fate/%$(OBJ_EXTENSION): src/fate/%.c | dirs
	@$(call MKDIR_P,$(@D))
	@echo " ==> $@"
	$(SEE_OBJ_CMD)$(CC) $(CCRELEASEFLAGS) $(if $(CAI_ENABLE),$(if $(filter $(CAI_EXCLUDE),$<),,$(CAI_CCFLAGS))) $(CCOBJ) $< $(CCOUT_OBJ)$@

$(BUILDDIR)/fate_dbg/%$(OBJ_EXTENSION): src/fate/%.c | dirs
	@$(call MKDIR_P,$(@D))
	@echo " ==> $@"
	$(SEE_OBJ_CMD)$(CC) $(CCDEBUGFLAGS) $(if $(CAI_ENABLE),$(if $(filter $(CAI_EXCLUDE),$<),,$(CAI_CCFLAGS))) $(CCOBJ) $< $(CCOUT_OBJ)$@
