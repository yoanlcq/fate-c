cflags = \
	-std=c11 \
	-idirafter $(fate)/include \
	-idirafter $(fate)/include/contrib \
	-Wall -Wno-format-security \
	--emrun \
	-s USE_SDL=2 -s ASYNCIFY=1 -s ASSERTIONS=2 -s EMTERPRETIFY=1 \
	-s EMTERPRETIFY_ASYNC=1 -s FORCE_FILESYSTEM=1 \
	$(foreach p,$(game_emscripten_preload),--preload-file $(p)@/memfs/$(p)) \
	$(foreach p,$(game_emscripten_embed),--embed-file $(p)@/memfs/$(p))

cflags_debug = -g -DFE_DEBUG_BUILD
glewflags = -DGLEW_STATIC -DGLEW_NO_GLU # -O0
cflags_release = -O3 -DNDEBUG
cc_c = -c 
cc_out_o = -o 
cc_out_exe = -o 
ldlibs=

$(eval $(foreach def,$(builds_$(build)_defines),cflags += -D$(def) ))
ifneq ($(builds_$(build)_debug),)
cflags += $(cflags_debug)
endif
ifneq ($(builds_$(build)_release),)
cflags += $(cflags_release)
endif

