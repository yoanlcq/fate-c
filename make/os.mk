define os_error

The os variable must be set to either windows, linux, or osx.

endef

ifeq ($(cc_id),emcc)
os:=web
endif

ifneq ($(os),windows)
ifneq ($(os),linux)
ifneq ($(os),osx)
ifneq ($(os),web)
$(error $(call os_error))
endif
endif
endif
endif
