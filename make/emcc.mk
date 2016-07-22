CCFLAGS = -std=c11 -idirafter include -idirafter include/contrib -Wall -Wno-format-security \
		 --emrun -s USE_SDL=2 -s ASYNCIFY=1 -s ASSERTIONS=1
CCDEBUGFLAGS = $(CCFLAGS) -O0 -g4 \
			   -DFE_DEBUG_BUILD -DFE_ENABLE_TRACING
GLEWFLAGS = -O0 -DGLEW_STATIC -DGLEW_NO_GLU
CCRELEASEFLAGS = $(CCFLAGS) -O3 -DNDEBUG
CCOBJ = -c 
CCOUT_OBJ = -o 
CCOUT_EXE = -o 
LDLIBS=
