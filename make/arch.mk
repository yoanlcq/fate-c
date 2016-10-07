define arch_error

The arch variable must be set to either 32 or 64.

endef

ifneq ($(os),osx)
ifneq ($(cc_id),emcc)
ifndef arch
$(error $(call arch_error))
else
ifneq ($(arch),32)
ifneq ($(arch),64)
$(error $(call arch_error))
endif
endif
endif
endif
endif
