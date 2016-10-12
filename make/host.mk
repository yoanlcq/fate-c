# See http://stackoverflow.com/a/12099167
ifeq ($(OS),Windows_NT)
	host_os:=windows
	ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
		host_arch:=64
	else
		ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
			host_arch:=64
		endif
		ifeq ($(PROCESSOR_ARCHITECTURE),x86)
			host_arch:=32
		endif
	endif
else
	uname_s:=$(shell uname -s)
	ifeq ($(uname_s),Linux)
		host_os:=linux
	endif
	ifeq ($(uname_s),Darwin)
		host_os:=osx
	endif
	uname_p := $(shell uname -p)
	ifeq ($(uname_p),x86_64)
		host_arch:=64
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		host_arch:=32
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		host_cpu:=arm
	endif
endif
