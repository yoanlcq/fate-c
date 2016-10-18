gm_src:=$(dirs_src)
gm_mkfiles:= $(call rglob,$(gm_src),*.mk)
gm_cfiles := $(call rglob,$(gm_src),*.c)
gm_sfiles := $(call rglob,$(gm_src),*$(dot_s))
gm_ofiles := \
 $(patsubst $(gm_src)/%.c,$(gm_build_dir)/%.c$(dot_o),$(gm_cfiles)) \
 $(patsubst $(gm_src)/%$(dot_s),$(gm_build_dir)/%$(dot_s)$(dot_o),$(gm_sfiles)) \
 $(fe_ofiles)

# here:=$(patsubst %/,%,$(dir $(f)))$(\n)\ 
$(eval $(foreach f,$(gm_mkfiles),include $(f)$(\n)))


$(gm_build_dir)/%.c$(dot_o): $(gm_src)/%.c
	@$(call mkdir_p,$(@D))
	@$(call echo,    $@)
	$(strip \
	$(see_obj_cmd)$(cc) $(cflags) \
	$(if $(filter $(fe_cai_blacklist),$<),,$(fe_cai_cflags)) \
	$(if $(filter $(sse4_2_whitelist),$<),$(sse4_2_cflags),) \
	$(cc_c) $< $(cc_out_o)$@ \
	)

$(gm_build_dir)/%$(dot_s)$(dot_o): $(fate)/src/%$(dot_s)
	@$(call mkdir_p,$(@D))
	@$(call echo,    $@)
	$(see_obj_cmd)$(as) $(as_c) $< $(as_out_o)$@

gm_bin:=$(dirs_bin)/$(triplet)
gm:=$(builds_$(build)_exename)
gmexe:=$(gm_bin)/$(gm)$(dot_exe)

ifeq ($(os),windows)
gm_rc  := $(gm_build_dir)/$(gm).rc

$(gm_rc):
	@$(call mkdir_p,$(@D))
	$(see_misc_cmd)$(cc) $(cc_pp) $(cflags) $(cc_pp_out)$@ \
		$(cc_D)game_windows_company_name="\"$(game_windows_company_name)\"" \
		$(cc_D)game_windows_file_version="\"$(game_windows_file_version)\"" \
		$(cc_D)game_windows_file_description="\"$(game_windows_file_description)\"" \
		$(cc_D)game_windows_internal_name="\"$(game_windows_internal_name)\"" \
		$(cc_D)game_windows_legal_copyright="\"$(game_windows_legal_copyright)\"" \
		$(cc_D)game_windows_legal_trademarks="\"$(game_windows_legal_trademarks)\"" \
		$(cc_D)game_windows_original_filename="\"$(game_windows_original_filename)\"" \
		$(cc_D)game_windows_product_name="\"$(game_windows_product_name)\"" \
		$(cc_D)game_windows_product_version="\"$(game_windows_product_version)\"" \
		$(cc_D)game_windows_icon="\"$(game_windows_icon)\"" \
		$(cc_lang_c) $(fate)/make/winrc.rc

$(gm_build_dir)/$(gm).res: $(gm_rc) $(game_windows_icon)
	@$(call mkdir_p,$(@D))
	$(see_misc_cmd)$(call windres_cmd,$<,$@)

gm_ofiles += $(gm_build_dir)/$(gm).res
endif

$(gmexe): $(gm_ofiles)
	@$(call mkdir_p,$(@D))
	@$(call echo,    $@)
	$(see_exe_cmd)$(cc) $(cflags) $^ $(cc_out_exe)$@ $(ldlibs)

game:=$(gmexe)

