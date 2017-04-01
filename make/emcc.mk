# Note to self :
# TOTAL_MEMORY must be a multiple of 16 Mb (16*1024*1024 bytes).
cflags = \
	-std=c11 \
	-idirafter $(fate)/include \
	-idirafter $(fate)/include/contrib \
	-Wall -Wno-format-security \
	--emrun \
	-s USE_SDL=2 \
	-s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png", "jpeg"]' \
	-s ASYNCIFY=1 -s ASSERTIONS=2 -s EMTERPRETIFY=1 \
	-s EMTERPRETIFY_ASYNC=1 -s FORCE_FILESYSTEM=1 \
	-s TOTAL_MEMORY=83886080 \
	-s OUTLINING_LIMIT=20000 \
	-s 'EMTERPRETIFY_FILE="emterpreter.bin"' \
	-s ALLOW_MEMORY_GROWTH=1 \
	--use-preload-plugins

#	-s USE_LIBPNG=1 \

# See https://kripken.github.io/emscripten-site/docs/compiling/Building-Projects.html#emscripten-ports
# Okay. DO NOT FRICKING COMPILE WITH -Os. This makes stbi_decode_from_memory() fail badly.
# Or perhaps -s STB_IMAGE=1 could change something.
cflags_debug = -g -O2 -DFE_DEBUG_BUILD
glewflags = -DGLEW_STATIC -DGLEW_NO_GLU # -O0
cflags_release = -O3 -DNDEBUG
cc_c = -c 
cc_out_o = -o 
cc_out_exe = -o 
ldlibs := \
	$(foreach p,$(game_emscripten_preload),--preload-file $(p)@/memfs/$(p)) \
	$(foreach p,$(game_emscripten_embed),--embed-file $(p)@/memfs/$(p))


$(eval $(foreach def,$(builds_$(build)_defines),cflags += -D$(def) ))
ifneq ($(builds_$(build)_debug),)
cflags += $(cflags_debug)
endif
ifneq ($(builds_$(build)_release),)
cflags += $(cflags_release)
endif

