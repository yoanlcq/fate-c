cflags = -std=c11 -Iinclude -Iinclude/contrib -Wall -D_GNU_SOURCE -msse -msse2
fe_cai_cflags := -finstrument-functions -finstrument-functions-exclude-file-list=x86intrin.h,emmintrin.h,smmintrin.h,immintrin.h

ifneq ($(arch),)
cflags += -m$(arch) 
endif

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
cflags_debug = $(cflags) -g -DFE_DEBUG_BUILD
cflags_release = $(cflags) -O3 -DNDEBUG
cc_c = -c 
cc_out_o = -o 
cc_out_exe = -o 

ifeq ($(cc),gcc) #can happen if we're coming from make/clang.mk.
ifeq ($(os),linux)
cflags_debug += -Og -rdynamic
endif
endif

ifeq ($(os),osx)
ldlibs += -framework sdl2 -framework opengl -lm -lintl -ldl
else
ldlibs += -lsdl2 -lm
ifneq ($(os),windows)
ldlibs += -lGL -ldl 
endif
endif

ifeq ($(os),windows)
cflags_release += -mwindows
ldlibs += -lopengl32 -lgdi32 -luser32 -lkernel32 -ldbghelp -lws2_32 
endif

