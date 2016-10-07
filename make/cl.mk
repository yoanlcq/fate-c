cflags = /nologo /Iinclude /Iinclude/contrib /Oi /DUNICODE /D_UNICODE /DDBGHELP_TRANSLATE_TCHAR
fe_cai_cflags := /Gh /GH 
#GLEWFLAGS = /DGLEW_STATIC /DGLEW_NO_GLU
cflags_debug = $(cflags) /Ob2 /Zi /DEBUG /FS /DFE_DEBUG_BUILD
cflags_release = $(cflags) /O2 /Ot /Ox /GL /Gw /DNDEBUG
ldlibs = \
 SDL2.lib SDL2main.lib opengl32.lib \
 gdi32.lib user32.lib Kernel32.lib DbgHelp.lib ws2_32.lib
ifneq ($(d3d10),)
ldlibs += d3d10.lib dxguid.lib
endif

cc_c = /c
cc_out_o = /Fo
cc_out_exe = /Fe

movepdb = move *.pdb $(bin)
