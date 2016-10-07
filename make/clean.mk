.PHONY: clean mrproper re

clean: 
	$(call rmdir_rf,$(build))
mrproper: clean all
re: mrproper
