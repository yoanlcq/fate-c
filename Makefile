.PHONY: all targets clean re mrproper
all: targets

fate:=$(subst \\,/,$(FE_PATH))
include fatefile.mk
include $(fate)/make/see_cmd.mk
include $(fate)/make/host.mk
include $(fate)/make/os_utils.mk
include $(fate)/make/installed_toolchains.mk
include $(fate)/make/post_fatefile.mk

targets: $(targets)

# Pass-through
clean: all
re: all
mrproper: all
# DO NOT DELETE
