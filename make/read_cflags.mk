read_cflags = $(strip $(if $(wildcard $(1).cflags),\
	$(subst $(cc_id):,,$(filter $(cc_id):%,\
		$(strip $(call readfile,$(1).cflags))\
	))\
	$(if $(findstring :no-cai,$(strip $(call readfile,$(1).cflags)))\
		,,$(fe_cai_cflags)\
	),\
$(fe_cai_cflags)))
