# Do not change this.
.PHONY: all dirs default_goals clean mrproper re

all: dirs default_goals
	
include make/os_arch_cc.mk
include make/$(CC).mk
include make/dirs.mk

# Changes start here.
# Be careful : There's a reason why there are no spaces between commas.

default_goals: game game_debug
	
$(eval $(call MKOBJ,game,glew.c,GL/glew.h,GL/glxew.h,GL/wglew.h))
$(eval $(call MKOBJ,game,fate/globalstate.c,fate/globalstate.h))
$(eval $(call MKOBJ,game,fate/log.c,fate/log.h))
$(eval $(call MKOBJ,game,fate/file_to_string.c,fate/file_to_string.h))
$(eval $(call MKOBJ,game,fate/sys.c,fate/sys.h))
$(eval $(call MKOBJ,game,fate/dpyres.c,fate/dpyres.h))
$(eval $(call MKOBJ,game,fate/gl/debug.c,fate/gl/debug.h))
$(eval $(call MKOBJ,game,fate/gl/mkprog.c,fate/gl/mkprog.h))
$(eval $(call MKOBJ,game,fate/gl/log.c,fate/gl/log.h))
$(eval $(call MKOBJ,game,cube/cube.c,../src/cube/cube.h))
$(eval $(call MKOBJ,game,cube/main.c))
$(eval $(call MKEXE,game))

