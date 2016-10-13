.PHONY: clean mrproper re

clean: 
	$(see_misc_cmd)$(call rmdir_rf,$(gm_build_dir))
	$(see_misc_cmd)$(call rmdir_rf,$(fe_build_dir))
mrproper: clean all
re: mrproper
