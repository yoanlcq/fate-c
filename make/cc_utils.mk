ifeq ($(cc_id),cl)
dot_o=.obj
dot_s=.asm
sse4_2_cflags:=
else
dot_o=.o
dot_s=.s
sse4_2_cflags:=-msse4.2
endif

ifeq ($(cc),emcc)
dot_o=.bc
dot_exe=.html
endif
