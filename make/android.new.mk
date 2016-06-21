ifeq ($(OS),android)

BUILDPATH=$(BUILDDIR)/$(GAME)
GAME_ACTIVITY=FateActivity
GAME_APP_NAME=Cube Demo
GAME_APP_DOMAIN=org.author.cube
GAME_APP_DOMAIN_AS_DIR=$(patsubst .,/,$(GAME_APP_DOMAIN))
NCPUS=4
ifeq ($(OS),linux)
NCPUS=$(shell nproc)
endif
ifeq ($(OS),osx)
NCPUS=$(shell sysctl -n hw.ncpu)
endif

$(BUILDPATH):
	$(call MKDIR_P,$@)
	cp -R dist/android_base/* $@

$(BUILDPATH)/jni/src/Android.mk: $(BUILDPATH)/jni/src/Android.mk.m4 | $(BUILDPATH)
	m4 -DGAME_SOURCE_FILES="$(GAME_SOURCE_FILES)" $< > $@
$(BUILDPATH)/AndroidManifest.xml: $(BUILDPATH)/AndroidManifest.xml.m4 | $(BUILDPATH)
	m4 -DGAME_APP_DOMAIN="$(GAME_APP_DOMAIN)" -DGAME_ACTIVITY=$(GAME_ACTIVITY) $< > $@
$(BUILDPATH)/build.xml: $(BUILDPATH)/build.xml.m4 | $(BUILDPATH)
	m4 -DGAME_APP_DOMAIN="$(GAME_APP_DOMAIN)" $< > $@
$(BUILDPATH)/res/values/strings.xml: $(BUILDPATH)/res/values/strings.xml.m4 | $(BUILDPATH)
	m4 -DGAME_APP_NAME="$(GAME_APP_NAME)" $< > $@


$(BUILDPATH)/jni/SDL: | $(BUILDPATH)
	$(call MKDIR_P,$@)
$(BUILDPATH)/jni/SDL/src: | $(BUILDPATH)/jni/SDL
	ln -s $(SDLPATH)/$(@F) $@
$(BUILDPATH)/jni/SDL/include: | $(BUILDPATH)/jni/SDL
	ln -s $(SDLPATH)/$(@F) $@
$(BUILDPATH)/jni/SDL/Android.mk: | $(BUILDPATH)/jni/SDL
	ln -s $(SDLPATH)/$(@F) $@


$(BUILDPATH)/jni/src/fate: | $(BUILDPATH)
	ln -s src/fate $@
$(BUILDPATH)/jni/include/fate: | $(BUILDPATH)
	ln -s include/fate $@
$(BUILDPATH)/jni/src/$(GAME): | $(BUILDPATH)
	ln -s src/$(GAME) $@


$(BUILDPATH)/src/$(GAME_APP_DOMAIN_AS_DIR):
	$(call MKDIR_P,$@)
$(BUILDPATH)/src/$(GAME_APP_DOMAIN_AS_DIR)/$(ACTIVITY).java: $(BUILDPATH)/MyActivity.java.m4 | $(BUILDPATH)/src/$(GAME_APP_DOMAIN_AS_DIR) 
	m4 -DGAME_APP_DOMAIN="$(GAME_APP_DOMAIN)" -DGAME_ACTIVITY="$(GAME_ACTIVITY)" $< > $@

GAME_DEBUG_APK=$(BUILDPATH)/bin/$(GAME_APP_DOMAIN)-debug.apk

$(GAME_DEBUG_APK): $(BUILDPATH)/jni/src/Android.mk \
				   $(BUILDPATH)/AndroidManifest.xml \
				   $(BUILDPATH)/build.xml \
				   $(BUILDPATH)/res/values/strings.xml \
				   $(BUILDPATH)/src/$(GAME_APP_DOMAIN_AS_DIR)/$(GAME_ACTIVITY).java \
				   $(BUILDPATH)/jni/SDL/Android.mk \
				 | $(BUILDPATH)/jni/SDL/src \
				   $(BUILDPATH)/jni/SDL/include\
				   $(BUILDPATH)/jni/src/fate \
				   $(BUILDPATH)/jni/src/$(GAME)
	android update project --path $(BUILDPATH)
	cd $(BUILDPATH) && ndk-build -j $(NCPUS) $(NDKARGS)
	cd $(BUILDPATH) && ant debug
	@echo "Test the app with: cd $(BUILDPATH) && ant debug install"

GAMES += $(GAME_DEBUG_APK)

endif
