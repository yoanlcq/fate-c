pwd=$(shell pwd)
cflags= -std=c99 -DFE_DEBUG_BUILD -DGLEW_STATIC \
		-DGLEW_NO_GLU -DFE_LINUXPERF_UNSUPPORTED
gm_rdomain_dir=$(subst .,/,$(gm_rdomain))
gm_cfiles= $(call rglob,\$$(LOCAL_PATH),*.c)


abase:=$(fate)/dist/android_base

$(gm_build_dir):
	$(call mkdir_p,$@)
	$(call cp_r,$(abase)/*,$@)
$(gm_build_dir)/res/values:
	$(call mkdir_p,$@)

$(gm_build_dir)/jni/src/Android.mk: $(abase)/m4/Android.mk.m4
	$(gnum4) -DGAME="$(gm)" -DGAME_CFLAGS="$(GAME_CFLAGS)" -DGAME_SOURCE_FILES="$(GAME_SOURCE_FILES)" $< > $@
$(gm_build_dir)/jni/src/main.c: $(abase)/m4/main.c.m4
	$(gnum4) -DGAME_MAIN="$(gm)_main" $< > $@
$(gm_build_dir)/AndroidManifest.xml: $(abase)/m4/AndroidManifest.xml.m4
	$(gnum4) -DGAME_APP_DOMAIN="$(GAME_APP_DOMAIN)" -DGAME_ACTIVITY=$(GAME_ACTIVITY) $< > $@
$(gm_build_dir)/build.xml: $(abase)/m4/build.xml.m4
	$(gnum4) -DGAME_APP_DOMAIN="$(GAME_APP_DOMAIN)" $< > $@
$(gm_build_dir)/res/%/ic_launcher.png: $(abase)/res/%/ic_launcher.png
	$(call cp_r,$<,$@)
$(gm_build_dir)/res/values/strings.xml: $(abase)/m4/strings.xml.m4
	$(gnum4) -DGAME_APP_NAME="$(GAME_APP_NAME)" $< > $@
$(gm_build_dir)/src/$(gm_rdomain_dir):
	$(call mkdir_p,$@)
$(gm_build_dir)/src/$(gm_rdomain_dir)/$(gm_activity).java: $(abase)/m4/MyActivity.java.m4
	$(gnum4) -DGAME_APP_DOMAIN="$(GAME_APP_DOMAIN)" -DGAME_ACTIVITY="$(GAME_ACTIVITY)" $< > $@


$(gm_build_dir)/jni/SDL: | $(gm_build_dir)
	$(call mkdir_p,$@)
$(gm_build_dir)/jni/SDL/src: | $(gm_build_dir)/jni/SDL
	$(call ln_sf,$(sdlpath)/$(@F),$@)
$(gm_build_dir)/jni/SDL/include: | $(gm_build_dir)/jni/SDL
	$(call ln_sf,$(sdlpath)/$(@F),$@)
$(gm_build_dir)/jni/SDL/Android.mk: | $(gm_build_dir)/jni/SDL
	$(call ln_sf,$(sdlpath)/$(@F),$@)

$(gm_build_dir)/jni/src/fate: | $(gm_build_dir)
	$(call ln_sf,$(pwd)/src/fate,$@)
$(gm_build_dir)/jni/src/$(gm): | $(gm_build_dir)
	$(call ln_sf,$(pwd)/src/$(gm),$@)
$(gm_build_dir)/jni/src/contrib: | $(gm_build_dir)
	$(call ln_sf,$(pwd)/src/contrib,$@)
$(gm_build_dir)/jni/include: | $(gm_build_dir)
	$(call mkdir_p,$@)
$(gm_build_dir)/jni/include/fate: | $(gm_build_dir)/jni/include
	$(call ln_sf,$(pwd)/include/fate,$@)
$(gm_build_dir)/jni/include/contrib: | $(gm_build_dir)/jni/include
	$(call ln_sf,$(pwd)/include/contrib,$@)
$(gm_build_dir)/jni/include/SDL2: | $(gm_build_dir)/jni/include
	$(call ln_sf,../SDL/include,$@)


game_debug_apk=$(gm_build_dir)/bin/$(GAME_APP_DOMAIN)-debug.apk


NDK_CPU_COUNT:=1
NDKARGS:= -j$(NDK_CPU_COUNT)

$(game_debug_apk): $(gm_build_dir)/jni/src/Android.mk \
				   $(gm_build_dir)/AndroidManifest.xml \
				   $(gm_build_dir)/build.xml \
				   $(gm_build_dir)/res/drawable-mdpi/ic_launcher.png \
				   $(gm_build_dir)/res/drawable-hdpi/ic_launcher.png \
				   $(gm_build_dir)/res/drawable-xhdpi/ic_launcher.png \
				   $(gm_build_dir)/res/drawable-xxhdpi/ic_launcher.png \
				   $(gm_build_dir)/res/values/strings.xml \
				   $(gm_build_dir)/src/$(gm_rdomain_dir)/$(gm_activity).java \
				   $(gm_build_dir)/jni/src/main.c \
				   $(gm_build_dir)/jni/SDL/Android.mk \
				 | $(gm_build_dir)/jni/SDL/src \
				   $(gm_build_dir)/jni/SDL/include\
				   $(gm_build_dir)/jni/src/fate \
				   $(gm_build_dir)/jni/src/contrib \
				   $(gm_build_dir)/jni/src/$(gm) \
				   $(gm_build_dir)/jni/include/fate \
				   $(gm_build_dir)/jni/include/contrib \
				   $(gm_build_dir)/jni/include/SDL2
	$(android) update project --path $(gm_build_dir)
	cd $(gm_build_dir) && $(ndk_build) $(NDKARGS)
	cd $(gm_build_dir) && $(ant) clean debug
	$(call echo,Test the app by specifying 'debug-install' as a recipe.)

game += $(game_debug_apk)

