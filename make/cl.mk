CCFLAGS = /nologo /Iinclude /Iinclude/contrib
CCDEBUGFLAGS = $(CCFLAGS) /Ob2 /Zi /DEBUG /DFATE_DEBUG_BUILD \
			   /DFATE_ENABLE_TRACING
CCRELEASEFLAGS = $(CCFLAGS) /O2 /Ot /Ox /GL /Gw /DNDEBUG
GLEWFLAGS = /DGLEW_STATIC /DGLEW_NO_GLU
LDLIBS = SDL2.lib SDL2main.lib opengl32.lib d3d10.lib dxguid.lib \
		 gdi32.lib user32.lib Kernel32.lib DbgHelp.lib
CCOBJ = /c
CCOUT_OBJ = /Fo
CCOUT_EXE = /Fe

MOVEPDB = move *.pdb $(BINDIR)
