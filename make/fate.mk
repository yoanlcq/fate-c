fe_mkfiles:= $(call rglob,$(fate)/src/fate,*.mk) \
             $(call rglob,$(fate)/src/contrib,*.mk)
fe_cfiles := $(call rglob,$(fate)/src/fate,*.c) \
	         $(call rglob,$(fate)/src/contrib,*.c)
fe_sfiles := $(call rglob,$(fate)/src/fate,*$(dot_s)) \
             $(call rglob,$(fate)/src/contrib,*$(dot_s))
fe_ofiles := \
 $(patsubst $(fate)/src/%.c,$(fe_build_dir)/%.c$(dot_o),$(fe_cfiles)) \
 $(patsubst $(fate)/src/%$(dot_s),$(fe_build_dir)/%$(dot_s)$(dot_o),$(fe_sfiles))

# here:=$(patsubst %/,%,$(dir $(f)))$(\n)\ 
$(eval $(foreach f,$(fe_mkfiles),include $(f)$(\n)))


$(fe_build_dir)/%.c$(dot_o): $(fate)/src/%.c
	@$(call mkdir_p,$(@D))
	@$(call echo,    $@)
	$(strip \
		$(see_obj_cmd)$(cc) $(cflags) \
		$(call read_cflags,$<) \
		$(cc_c) $< $(cc_out_o)$@ \
	)

$(fe_build_dir)/%$(dot_s)$(dot_o): $(fate)/src/%$(dot_s)
	@$(call mkdir_p,$(@D))
	@$(call echo,    $@)
	$(see_obj_cmd)$(as) $(as_out_o)$@ $(as_c) $<
