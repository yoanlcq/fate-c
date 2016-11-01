fe_idir :=$(subst /,\\,$(fate)/include)
fe_cidir:=$(subst /,\\,$(fate)/include/contrib)
gm_idir :=$(subst /,\\,$(dirs_include))
cflags = /I$(fe_idir) /I$(fe_cidir) /I$(gm_idir) /Oi /DUNICODE /D_UNICODE /DDBGHELP_TRANSLATE_TCHAR /Wall /wd4255 /wd4820 /wd4668 /wd4214 /wd4201 /wd4204
#GLEWFLAGS = /DGLEW_STATIC /DGLEW_NO_GLU
cflags_debug = /Ob2 /Zi /Zo /DEBUG /FS /DFE_DEBUG_BUILD
cflags_release = /O2 /Ot /Ox /GL /Gw /DNDEBUG
ldlibs = \
 SDL2.lib SDL2main.lib opengl32.lib \
 gdi32.lib user32.lib Kernel32.lib DbgHelp.lib ws2_32.lib shell32.lib ole32.lib
ifneq ($(d3d10),)
ldlibs += d3d10.lib dxguid.lib
endif

cc_pp = /EP /P 
cc_pp_out = /Fi
cc_lang_c = /TC 
cc_D = /D
cc_c = /c
cc_out_o = /Fo
cc_out_exe = /Fe
as_c = /c
as_out_o = /Fo

$(eval $(foreach def,$(builds_$(build)_defines),cflags += /D$(def) ))
ifneq ($(builds_$(build)_debug),)
cflags += $(cflags_debug)
endif
ifneq ($(builds_$(build)_release),)
cflags += $(cflags_release)
endif
ifneq ($(builds_$(build)_cai),)
fe_cai_cflags := /Gh /GH 
endif

