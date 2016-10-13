define cc_id_error

The cc_id variable must be set to either gcc, cl, clang or emcc.

endef

ifneq ($(cc_id),gcc)
ifneq ($(cc_id),clang)
ifneq ($(cc_id),cl)
ifneq ($(cc_id),emcc)
$(error $(call cc_id_error))
endif
endif
endif
endif
