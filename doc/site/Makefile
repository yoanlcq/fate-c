.PHONY: all clean re mrproper

all: index.html

contents/Makefile: content_list.m4 components/_content_list.m4 components/content_list_.m4
	m4 $< > $@

index.html: contents/Makefile
	$(MAKE) -j$(shell nproc) -C$(dir $<) -f $(notdir $<) $(MAKEFLAGS) $(MAKECMDGOALS) \
		&& m4 -Dlc=en -Dget_root=. index.m4 > index.html

clean: 
	rm -f index.html contents/Makefile
re: clean all
mrproper: re
