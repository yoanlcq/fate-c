fe_mkfiles:= $(call rglob,$(fate)/src/fate,*.mk) \
             $(call rglob,$(fate)/src/contrib,*.mk)
fe_cfiles := $(call rglob,$(fate)/src/fate,*.c) \
	         $(call rglob,$(fate)/src/contrib,*.c)
fe_sfiles := $(call rglob,$(fate)/src/fate,*$(dot_s)) \
             $(call rglob,$(fate)/src/contrib,*$(dot_s))
fe_ofiles := \
 $(patsubst $(fate)/src/%.c,$(fe_build_dir)/%.c$(dot_o),$(fe_cfiles)) \
 $(patsubst $(fate)/src/%$(dot_s),$(fe_build_dir)/%$(dot_s)$(dot_o),$(fe_sfiles))

$(eval $(foreach f,$(fe_mkfiles),here:=$(patsubst %/,%,$(dir $(f)))$(\n)include $(f)$(\n)))

$(info [INFO] fe_cai_blacklist is $(fe_cai_blacklist))
$(info [INFO] sse4_2_whitelist is $(sse4_2_whitelist))

$(fe_build_dir)/%.c$(dot_o): $(fate)/src/%.c
	@$(call mkdir_p,$(@D))
	@$(call echo,    $@)
	foo $< vs $(sse4_2_whitelist)
	$(strip \
	$(see_obj_cmd)$(cc) $(cflags) \
	$(if $(filter $(fe_cai_blacklist),$<),,$(fe_cai_cflags)) \
	$(if $(filter $(sse4_2_whitelist),$<),$(sse4_2_cflags),) \
	$(cc_c) $< $(cc_out_o)$@ \
	)

$(fe_build_dir)/%$(dot_s)$(dot_o): $(fate)/src/%$(dot_s)
	@$(call mkdir_p,$(@D))
	@$(call echo,    $@)
	$(see_obj_cmd)$(as) $(as_c) $< $(as_out_o)$@
