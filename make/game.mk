gm_mkfiles:= $(call rglob,$(gm_src),*.mk)
gm_cfiles := $(call rglob,$(gm_src),*.c)
gm_sfiles := $(call rglob,$(gm_src),*$(dot_s))
gm_ofiles := \
 $(patsubst $(gm_src)/%.c,$(gm_build)/%.c$(dot_o),$(gm_cfiles)) \
 $(patsubst $(gm_src)/%$(dot_s),$(gm_build)/%$(dot_s)$(dot_o),$(gm_sfiles)) \
 $(fe_ofiles)

$(eval $(foreach f,$(gm_mkfiles),include $(f)))

$(gm_build)/%.c$(dot_o): $(gm_src)/%.c
	@$(call mkdir,$(@D))
	@$(call echo, ==> $@)
	$(strip 
	$(see_obj_cmd)$(cc) $(cflags) \
	$(if $(filter $(fe_cai_blacklist),$<),,$(fe_cai_cflags)) \
	$(if $(filter $(sse4_2_whitelist),$<),$(sse4_2_cflags),) \
	$(cc_c) $< $(cc_out_o)$@ \
	)

$(gm_build)/%$(dot_s)$(dot_o): $(fate)/src/%$(dot_s)
	@$(call mkdir,$(@D))
	@$(call echo, ==> $@)
	$(see_obj_cmd)$(as) $(as_c) $< $(as_out_o)$@

ifeq ($(os),windows)
$(gm_build)/$(gm).res: $(gm_rc) $(gm_ico)
	@$(call mkdir,$(@D))
	$(see_misc_cmd)$(call windres_cmd,$<,$@)
endif

$(gm_bin)/$(gm): $(gm_ofiles)
	@$(call echo, ==> $@)
	$(see_exe_cmd)$(cc) $(cflags) $^ $(cc_out_exe)$@ $(ldlibs)

game:=$(gm_bin)/$(gm)

