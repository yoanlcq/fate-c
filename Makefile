# Do not change this.
.PHONY: all dirs default_goals clean mrproper re

all: dirs default_goals
	
include make/os_arch_cc.mk
include make/$(CC).mk
include make/dirs.mk

# Changes start here.
# Be careful : There's a reason why there are no spaces between commas.

default_goals: foo foo_debug
	
$(eval $(call MKOBJ,game,glew.c,GL/glew.h,GL/glxew.h,GL/wglew.h))
$(eval $(call MKEXE,game))

$(eval $(call MKOBJ,foo,test))
$(eval $(call MKEXE,foo))

