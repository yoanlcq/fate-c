.PHONY: all targets
all: targets

fate:=$(FE_PATH)
include fatefile.mk
include $(fate)/make/host.mk
include $(fate)/make/installed_toolchains.mk
include $(fate)/make/post_fatefile.mk

targets: $(targets)
