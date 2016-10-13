cflags = -std=c11 -I$(fate)/include -I$(fate)/include/contrib -I$(dirs_include) -Wall -D_GNU_SOURCE -msse -msse2

# C'mon travis
ifeq ($(os),linux)
ifneq ($(shell cat /usr/include/linux/hw_breakpoint.h > /dev/null 2>&1; echo $$?),0)
cflags += -DFE_LINUXPERF_UNSUPPORTED
endif
endif

ifeq ($(os),windows)
cflags += -DUNICODE -D_UNICODE -DDBGHELP_TRANSLATE_TCHAR
endif

ifeq ($(os),osx)
cflags += -F/library/frameworks 
endif

ifneq ($(vulkan),)
ifneq ($(os),osx)
cflags += -DFE_USE_VULKAN 
ifeq ($(os),linux)
ldlibs += -lvulkan
endif
ifeq ($(os),windows)
ldlibs += -lvulkan-1
endif
endif
endif

ifneq ($(d3d10),)
ifeq ($(os),windows)
ldlibs += -ld3d10 -ldxguid 
endif
endif

#glewflags = -DGLEW_STATIC -DGLEW_NO_GLU
cflags_debug = -g -DFE_DEBUG_BUILD
cflags_release = -O3 -DNDEBUG
cc_c = -c 
cc_out_o = -o 
cc_out_exe = -o 
as_c = -c 
as_out_o = -o 

ifeq ($(cc_id),gcc) #can happen if we're coming from make/clang.mk.
ifeq ($(os),linux)
cflags_debug += -Og -rdynamic
endif
endif

ifeq ($(os),osx)
ldlibs += -framework SDL2 -framework OpenGL -lm -lintl -ldl
else
ldlibs += -lSDL2 -lm
ifneq ($(os),windows)
ldlibs += -lGL -ldl 
endif
endif

ifeq ($(os),windows)
ldlibs += -lopengl32 -lgdi32 -luser32 -lkernel32 -ldbghelp -lws2_32 
endif

$(eval $(foreach def,$(builds_$(build)_defines),cflags += -D$(def) ))
ifneq ($(builds_$(build)_debug),)
cflags += $(cflags_debug)
endif
ifneq ($(builds_$(build)_release),)
cflags += $(cflags_release)
endif
ifneq ($(builds_$(build)_cai),)
fe_cai_cflags := -finstrument-functions 
ifeq ($(cc_id),gcc) #can happen if we're coming from make/clang.mk.
fe_cai_cflags += -finstrument-functions-exclude-file-list=x86intrin.h,emmintrin.h,smmintrin.h,immintrin.h,xmmintrin.h
endif
endif

ifeq ($(os),windows)
ifneq ($(builds_$(build)_mingw_gui_app),)
cflags += -mwindows 
endif
endif

