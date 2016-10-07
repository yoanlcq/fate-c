ifeq ($(os),windows)
gnumake:=mingw32-make
else
gnumake:=make
endif

gnum4:=m4 --gnu 

cc:=gcc
as:=as
ifeq ($(cc_id),cl)
cc:=cl /nologo
as:=ml /nologo /Dx86
ifeq ($(arch),64)
as:=ml64 /nologo /Dx64
endif
endif

android:=android
ndk_build:=ndk-build
ant:=ant
