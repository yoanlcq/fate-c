BUILDPATH=build/android/$(GAME)
NDKARGS= 
PWD=$(shell pwd)
GAME_CFLAGS= -std=c99 -DFE_DEBUG_BUILD -DGLEW_STATIC -DGLEW_NO_GLU -DFE_LINUXPERF_UNSUPPORTED
GAME_ACTIVITY=FateActivity
GAME_APP_NAME=Cube Demo
GAME_APP_DOMAIN=org.author.cube
GAME_APP_DOMAIN_AS_DIR=org/author/cube
GAME_SOURCE_FILES= \$$(LOCAL_PATH)/main.c \
				   \$$(wildcard \$$(LOCAL_PATH)/$(GAME)/*.c \
				  			    \$$(LOCAL_PATH)/$(GAME)/*/*.c \
				  			    \$$(LOCAL_PATH)/$(GAME)/*/*/*.c \
							    \$$(LOCAL_PATH)/$(GAME)/*/*/*/*.c \
							    \$$(LOCAL_PATH)/$(GAME)/*/*/*/*/*.c \
							    \$$(LOCAL_PATH)/$(GAME)/*/*/*/*/*/*.c \
				   				\$$(LOCAL_PATH)/fate/*.c \
				  			    \$$(LOCAL_PATH)/fate/*/*.c \
				  			    \$$(LOCAL_PATH)/fate/*/*/*.c \
							    \$$(LOCAL_PATH)/fate/*/*/*/*.c \
							    \$$(LOCAL_PATH)/fate/*/*/*/*/*.c \
							    \$$(LOCAL_PATH)/fate/*/*/*/*/*/*.c \
				   				\$$(LOCAL_PATH)/contrib/*.c \
				  			    \$$(LOCAL_PATH)/contrib/*/*.c \
				  			    \$$(LOCAL_PATH)/contrib/*/*/*.c \
							    \$$(LOCAL_PATH)/contrib/*/*/*/*.c \
							    \$$(LOCAL_PATH)/contrib/*/*/*/*/*.c \
							    \$$(LOCAL_PATH)/contrib/*/*/*/*/*/*.c)

ifndef SDLPATH
$(error Please define the SDLPATH variable to point to your SDL2-2.0.4 directory)
endif

NCPUS=4
ifeq ($(OS),linux)
NCPUS=$(shell nproc)
endif
ifeq ($(OS),osx)
NCPUS=$(shell sysctl -n hw.ncpu)
endif

$(BUILDPATH):
	mkdir -p $@
	cp -R dist/android_base/* $@
$(BUILDPATH)/res/values:
	mkdir -p $@

$(BUILDPATH)/jni/src/Android.mk: dist/android_base/m4/Android.mk.m4 | $(BUILDPATH)
	m4 -DGAME="$(GAME)" -DGAME_CFLAGS="$(GAME_CFLAGS)" -DGAME_SOURCE_FILES="$(GAME_SOURCE_FILES)" $< > $@
$(BUILDPATH)/jni/src/main.c: dist/android_base/m4/main.c.m4 | $(BUILDPATH)
	m4 -DGAME_MAIN="$(GAME)_main" $< > $@
$(BUILDPATH)/AndroidManifest.xml: dist/android_base/m4/AndroidManifest.xml.m4 | $(BUILDPATH)
	m4 -DGAME_APP_DOMAIN="$(GAME_APP_DOMAIN)" -DGAME_ACTIVITY=$(GAME_ACTIVITY) $< > $@
$(BUILDPATH)/build.xml: dist/android_base/m4/build.xml.m4 | $(BUILDPATH)
	m4 -DGAME_APP_DOMAIN="$(GAME_APP_DOMAIN)" $< > $@
$(BUILDPATH)/res/values/strings.xml: dist/android_base/m4/strings.xml.m4 | $(BUILDPATH) $(BUILDPATH)/res/values
	m4 -DGAME_APP_NAME="$(GAME_APP_NAME)" $< > $@
$(BUILDPATH)/src/$(GAME_APP_DOMAIN_AS_DIR):
	mkdir -p $@
$(BUILDPATH)/src/$(GAME_APP_DOMAIN_AS_DIR)/$(GAME_ACTIVITY).java: dist/android_base/m4/MyActivity.java.m4 | $(BUILDPATH)/src/$(GAME_APP_DOMAIN_AS_DIR) 
	m4 -DGAME_APP_DOMAIN="$(GAME_APP_DOMAIN)" -DGAME_ACTIVITY="$(GAME_ACTIVITY)" $< > $@


$(BUILDPATH)/jni/SDL: | $(BUILDPATH)
	mkdir -p $@
$(BUILDPATH)/jni/SDL/src: | $(BUILDPATH)/jni/SDL
	ln -sf $(SDLPATH)/$(@F) $@
$(BUILDPATH)/jni/SDL/include: | $(BUILDPATH)/jni/SDL
	ln -sf $(SDLPATH)/$(@F) $@
$(BUILDPATH)/jni/SDL/Android.mk: | $(BUILDPATH)/jni/SDL
	ln -sf $(SDLPATH)/$(@F) $@

$(BUILDPATH)/jni/src/fate: | $(BUILDPATH)
	ln -sf $(PWD)/src/fate $@
$(BUILDPATH)/jni/src/$(GAME): | $(BUILDPATH)
	ln -sf $(PWD)/src/$(GAME) $@
$(BUILDPATH)/jni/src/contrib: | $(BUILDPATH)
	ln -sf $(PWD)/src/contrib $@
$(BUILDPATH)/jni/include: | $(BUILDPATH)
	mkdir -p $@
$(BUILDPATH)/jni/include/fate: | $(BUILDPATH)/jni/include
	ln -sf $(PWD)/include/fate $@
$(BUILDPATH)/jni/include/contrib: | $(BUILDPATH)/jni/include
	ln -sf $(PWD)/include/contrib $@
$(BUILDPATH)/jni/include/SDL2: | $(BUILDPATH)/jni/include
	ln -sf ../SDL/include $@


GAME_DEBUG_APK=$(BUILDPATH)/bin/$(GAME_APP_DOMAIN)-debug.apk


#NCPUS=1

$(GAME_DEBUG_APK): $(BUILDPATH)/jni/src/Android.mk \
				   $(BUILDPATH)/AndroidManifest.xml \
				   $(BUILDPATH)/build.xml \
				   $(BUILDPATH)/res/values/strings.xml \
				   $(BUILDPATH)/src/$(GAME_APP_DOMAIN_AS_DIR)/$(GAME_ACTIVITY).java \
				   $(BUILDPATH)/jni/src/main.c \
				   $(BUILDPATH)/jni/SDL/Android.mk \
				 | $(BUILDPATH)/jni/SDL/src \
				   $(BUILDPATH)/jni/SDL/include\
				   $(BUILDPATH)/jni/src/fate \
				   $(BUILDPATH)/jni/src/contrib \
				   $(BUILDPATH)/jni/src/$(GAME) \
				   $(BUILDPATH)/jni/include/fate \
				   $(BUILDPATH)/jni/include/contrib \
				   $(BUILDPATH)/jni/include/SDL2
	android update project --path $(BUILDPATH)
	cd $(BUILDPATH) && ndk-build -j$(NCPUS) $(NDKARGS)
	cd $(BUILDPATH) && ant clean debug
	@echo "Test the app with: cd $(BUILDPATH) && ant debug install"

GAMES += $(GAME_DEBUG_APK)

