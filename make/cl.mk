CCFLAGS = /nologo /Iinclude /Iinclude/contrib /Oi /DUNICODE /D_UNICODE /DDBGHELP_TRANSLATE_TCHAR
CAI_CCFLAGS := /Gh /GH 
GLEWFLAGS = /DGLEW_STATIC /DGLEW_NO_GLU
CCDEBUGFLAGS = $(CCFLAGS) /Ob2 /Zi /DEBUG /FS /DFE_DEBUG_BUILD \
			   /DFE_ENABLE_TRACING $(GLEWFLAGS)
CCRELEASEFLAGS = $(CCFLAGS) /O2 /Ot /Ox /GL /Gw /DNDEBUG $(GLEWFLAGS)
LDLIBS = SDL2.lib SDL2main.lib opengl32.lib d3d10.lib dxguid.lib \
		 gdi32.lib user32.lib Kernel32.lib DbgHelp.lib ws2_32.lib #libintl.lib
CCOBJ = /c
CCOUT_OBJ = /Fo
CCOUT_EXE = /Fe

MOVEPDB = move *.pdb $(BINDIR)
