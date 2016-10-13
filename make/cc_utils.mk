ifeq ($(cc_id),cl)
dot_o=.obj
ifeq ($(arch),64)
dot_s=.x64.asm
else
dot_s=.x86.asm
endif
sse4_2_cflags:=
else
dot_o=.o
ifeq ($(arch),64)
dot_s=.x64.s
else
dot_s=.x86.s
endif
sse4_2_cflags:=-msse4.2
endif

ifeq ($(cc_id),emcc)
dot_o=.bc
dot_exe=.html
endif
