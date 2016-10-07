cpu_count:=1
ifeq ($(OS),linux)
cpu_count:=$(shell nproc)
endif
ifeq ($(OS),osx)
cpu_count:=$(shell sysctl -n hw.ncpu)
endif
