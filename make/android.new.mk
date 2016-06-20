ifeq ($(OS),android)

BUILDPATH=$(BUILDDIR)/$(GAME)
ACTIVITY=FateActivity
APP_NAME=Cube Demo
APP_DOMAIN=org.author.cube
APP_DOMAIN_AS_DIR=$(patsubst .,/,$(APP_DOMAIN))
NCPUS=4
ifeq ($(OS),linux)
NCPUS=$(shell nproc)
endif
ifeq ($(OS),osx)
NCPUS=$(shell sysctl -n hw.ncpu)
endif

$(BUILDPATH):
	$(call MKDIR_P,$@)
	cp -R $(SDLPATH)/android-project/* $@
	sed -i "s|YourSourceHere.c|$(MY_SOURCE_FILES)|g" $(BUILDPATH)/jni/src/Android.mk
	sed -i "s|org\.libsdl\.app|$(APP_DOMAIN)|g"      $(BUILDPATH)/AndroidManifest.xml
	sed -i "s|SDLActivity|$(ACTIVITY)|g"             $(BUILDPATH)/AndroidManifest.xml
	sed -i "s|SDLActivity|$(APP_DOMAIN)|g"           $(BUILDPATH)/build.xml
	sed -i "s|SDL App|$(APP_NAME)|g"                 $(BUILDPATH)/res/values/strings.xml

$(BUILDPATH)/jni/SDL: | $(BUILDPATH)
	$(call MKDIR_P,$@)
$(BUILDPATH)/jni/SDL/src: | $(BUILDPATH)/jni/SDL
	ln -s $(SDLPATH)/$(@F) $@
$(BUILDPATH)/jni/SDL/include: | $(BUILDPATH)/jni/SDL
	ln -s $(SDLPATH)/$(@F) $@
$(BUILDPATH)/jni/SDL/Android.mk: | $(BUILDPATH)/jni/SDL
	ln -s $(SDLPATH)/$(@F) $@


$(BUILDPATH)/jni/src/fate: | $(BUILDPATH)
	$(call MKDIR_P,$@)
	ln -s src/fate $@
$(BUILDPATH)/jni/src/$(GAME): | $(BUILDPATH)
	$(call MKDIR_P,$@)
	ln -s src/$(GAME) $@


$(BUILDPATH)/src/$(APP_DOMAIN_AS_DIR):
	$(call MKDIR_P,$@)
$(BUILDPATH)/src/$(APP_DOMAIN_AS_DIR)/$(ACTIVITY).java: | $(BUILDPATH)/src/$(APP_DOMAIN_AS_DIR) 
	echo "package $(APP_DOMAIN);" > $@
	echo "import org.libsdl.app.SDLActivity;" >> $@
	echo "public class $(ACTIVITY) extends SDLActivity {}" >> $@

GAME_DEBUG_APK=$(BUILDPATH)/bin/$(APP_DOMAIN)-debug.apk

$(GAME_DEBUG_APK): $(BUILDPATH)/src/$(APP_DOMAIN_AS_DIR)/$(ACTIVITY).java \
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
