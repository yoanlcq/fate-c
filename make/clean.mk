.PHONY: clean mrproper re

clean: 
	$(call rmdir_rf,$(gm_build_dir))
	$(call rmdir_rf,$(fe_build_dir))
mrproper: clean all
re: mrproper
