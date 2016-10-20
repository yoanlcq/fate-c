ifeq ($(arch),32)
windres_target=pe-i386
else
windres_target=pe-x86-64
endif
ifeq ($(cc_id),cl)
windres_cmd=$(rc) /fo $(subst /,\\,$(2)) $(subst /,\\,$(1))
else
windres_cmd=$(windres) $(1) -O coff -F $(windres_target) -o $(2)
endif

