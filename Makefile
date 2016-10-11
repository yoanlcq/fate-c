.PHONY: all targets
all: targets

fate:=$(FE_PATH)
include fatefile.mk
include $(fate)/installed/$(host_os)/toolchains.mk
include $(fate)/make/post_fatefile.mk

targets: $(targets)
