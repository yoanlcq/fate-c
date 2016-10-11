fe_mkfiles:= $(call rglob,$(fate)/src,*.mk)
fe_cfiles := $(call rglob,$(fate)/src,*.c)
fe_sfiles := $(call rglob,$(fate)/src,*$(dot_s))
fe_ofiles := \
 $(patsubst $(fate)/src/%.c,$(fate)/$(fe_build_dir)/%.c$(dot_o),$(fe_cfiles)) \
 $(patsubst $(fate)/src/%$(dot_s),$(fate)/$(fe_build_dir)/%$(dot_s)$(dot_o),$(fe_sfiles))

$(eval $(foreach f,$(fe_mkfiles),
here:=$(dir $(f)) 
include $(f)
))

$(fate)/$(fe_build_dir)/%.c$(dot_o): $(fate)/src/%.c
	@$(call mkdir,$(@D))
	@$(call echo, ==> $@)
	$(strip \
	$(see_obj_cmd)$(cc) $(cflags) \
	$(if $(filter $(fe_cai_blacklist),$<),,$(fe_cai_cflags)) \
	$(if $(filter $(sse4_2_whitelist),$<),$(sse4_2_cflags),) \
	$(cc_c) $< $(cc_out_o)$@ \
	)

$(fate)/$(fe_build_dir)/%$(dot_s)$(dot_o): $(fate)/src/%$(dot_s)
	@$(call mkdir,$(@D))
	@$(call echo, ==> $@)
	$(see_obj_cmd)$(as) $(as_c) $< $(as_out_o)$@
