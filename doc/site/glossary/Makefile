.PHONY: all clean re mrproper

all: properties.m4

ENTRIES = $(sort entries/*.m4)

properties.m4: $(ENTRIES)
	echo "include(get_root/\`components/glossary_.m4')dnl" > $@
	echo $(foreach entry,$^,"include(\`$(entry)')dnl") >> $@
	echo "include(get_root/\`components/glossary_.m4')dnl" >> $@


clean:
	rm -f properties.m4
re: clean all
mrproper: re
