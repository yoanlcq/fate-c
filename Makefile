# Do not change this.
.PHONY: all dirs default_goals clean mrproper re

all: dirs default_goals
	
include make/os_arch_cc.mk
include make/$(CC).mk
include make/dirs.mk

# Changes start here.
# Be careful : There's a reason why there are no spaces between commas.

default_goals: fate_cube_demo fate_cube_demo_debug 

#fate_d3d10_sample fate_d3d10_sample_debug
	
$(eval $(call MKRES,fate_cube_demo))
$(eval $(call MKOBJ,fate_cube_demo,glew.c,GL/glew.h,GL/glxew.h,GL/wglew.h))
$(eval $(call MKOBJ,fate_cube_demo,fate/globalstate.c,fate/globalstate.h))
$(eval $(call MKOBJ,fate_cube_demo,fate/fatal_alloc.c,fate/fatal_alloc.h))
$(eval $(call MKOBJ,fate_cube_demo,fate/log.c,fate/log.h))
$(eval $(call MKOBJ,fate_cube_demo,fate/file_to_string.c,fate/file_to_string.h))
$(eval $(call MKOBJ,fate_cube_demo,fate/sys.c,fate/sys.h))
$(eval $(call MKOBJ,fate_cube_demo,fate/dpyres.c,fate/dpyres.h))
$(eval $(call MKOBJ,fate_cube_demo,fate/gl/debug.c,fate/gl/debug.h))
$(eval $(call MKOBJ,fate_cube_demo,fate/gl/mkprog.c,fate/gl/mkprog.h))
$(eval $(call MKOBJ,fate_cube_demo,fate/gl/log.c,fate/gl/log.h))
$(eval $(call MKOBJ,fate_cube_demo,fate/gl/meminfo.c,fate/gl/meminfo.h))
$(eval $(call MKOBJ,fate_cube_demo,fate/gl/stats.c,fate/gl/stats.h))
$(eval $(call MKOBJ,fate_cube_demo,fate/gl/utils.c,fate/gl/utils.h))
$(eval $(call MKOBJ,fate_cube_demo,cube/cube.c,../src/cube/cube.h))
$(eval $(call MKOBJ,fate_cube_demo,cube/main.c))
$(eval $(call MKEXE,fate_cube_demo))

ifeq ($(OS),windows)
$(eval $(call MKOBJ,fate_d3d10_sample,fate/d3d/d3d10_sample.c))
$(eval $(call MKEXE,fate_d3d10_sample))
else
fate_d3d10_sample: ;
fate_d3d10_sample_debug: ;
.PHONY: fate_d3d10_sample fate_d3d10_sample_debug
endif

