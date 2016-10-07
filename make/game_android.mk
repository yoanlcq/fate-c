pwd=$(shell pwd)
cflags= -std=c99 -DFE_DEBUG_BUILD -DGLEW_STATIC \
		-DGLEW_NO_GLU -DFE_LINUXPERF_UNSUPPORTED
gm_rdomain_dir=$(subst .,/,$(gm_rdomain))
gm_cfiles= $(call rglob,\$$(LOCAL_PATH),*.c)


abase:=$(fate)/dist/android_base

$(build):
	$(call mkdir,$@)
	$(call cp_r,$(abase)/*,$@)
$(build)/res/values:
	$(call mkdir,$@)

$(build)/jni/src/Android.mk: $(abase)/m4/Android.mk.m4
	$(gnum4) -DGAME="$(gm)" -DGAME_CFLAGS="$(GAME_CFLAGS)" -DGAME_SOURCE_FILES="$(GAME_SOURCE_FILES)" $< > $@
$(build)/jni/src/main.c: $(abase)/m4/main.c.m4
	$(gnum4) -DGAME_MAIN="$(gm)_main" $< > $@
$(build)/AndroidManifest.xml: $(abase)/m4/AndroidManifest.xml.m4
	$(gnum4) -DGAME_APP_DOMAIN="$(GAME_APP_DOMAIN)" -DGAME_ACTIVITY=$(GAME_ACTIVITY) $< > $@
$(build)/build.xml: $(abase)/m4/build.xml.m4
	$(gnum4) -DGAME_APP_DOMAIN="$(GAME_APP_DOMAIN)" $< > $@
$(build)/res/%/ic_launcher.png: $(abase)/res/%/ic_launcher.png
	$(call cp_r,$<,$@)
$(build)/res/values/strings.xml: $(abase)/m4/strings.xml.m4
	$(gnum4) -DGAME_APP_NAME="$(GAME_APP_NAME)" $< > $@
$(build)/src/$(gm_rdomain_dir):
	$(call mkdir,$@)
$(build)/src/$(gm_rdomain_dir)/$(gm_activity).java: $(abase)/m4/MyActivity.java.m4
	$(gnum4) -DGAME_APP_DOMAIN="$(GAME_APP_DOMAIN)" -DGAME_ACTIVITY="$(GAME_ACTIVITY)" $< > $@


$(build)/jni/SDL: | $(build)
	$(call mkdir,$@)
$(build)/jni/SDL/src: | $(build)/jni/SDL
	$(call ln_sf,$(sdlpath)/$(@F),$@)
$(build)/jni/SDL/include: | $(build)/jni/SDL
	$(call ln_sf,$(sdlpath)/$(@F),$@)
$(build)/jni/SDL/Android.mk: | $(build)/jni/SDL
	$(call ln_sf,$(sdlpath)/$(@F),$@)

$(build)/jni/src/fate: | $(build)
	$(call ln_sf,$(pwd)/src/fate,$@)
$(build)/jni/src/$(gm): | $(build)
	$(call ln_sf,$(pwd)/src/$(gm),$@)
$(build)/jni/src/contrib: | $(build)
	$(call ln_sf,$(pwd)/src/contrib,$@)
$(build)/jni/include: | $(build)
	$(call mkdir,$@)
$(build)/jni/include/fate: | $(build)/jni/include
	$(call ln_sf,$(pwd)/include/fate,$@)
$(build)/jni/include/contrib: | $(build)/jni/include
	$(call ln_sf,$(pwd)/include/contrib,$@)
$(build)/jni/include/SDL2: | $(build)/jni/include
	$(call ln_sf,../SDL/include,$@)


game_debug_apk=$(build)/bin/$(GAME_APP_DOMAIN)-debug.apk


NDK_CPU_COUNT:=1
NDKARGS:= -j$(NDK_CPU_COUNT)

$(game_debug_apk): $(build)/jni/src/Android.mk \
				   $(build)/AndroidManifest.xml \
				   $(build)/build.xml \
				   $(build)/res/drawable-mdpi/ic_launcher.png \
				   $(build)/res/drawable-hdpi/ic_launcher.png \
				   $(build)/res/drawable-xhdpi/ic_launcher.png \
				   $(build)/res/drawable-xxhdpi/ic_launcher.png \
				   $(build)/res/values/strings.xml \
				   $(build)/src/$(gm_rdomain_dir)/$(gm_activity).java \
				   $(build)/jni/src/main.c \
				   $(build)/jni/SDL/Android.mk \
				 | $(build)/jni/SDL/src \
				   $(build)/jni/SDL/include\
				   $(build)/jni/src/fate \
				   $(build)/jni/src/contrib \
				   $(build)/jni/src/$(gm) \
				   $(build)/jni/include/fate \
				   $(build)/jni/include/contrib \
				   $(build)/jni/include/SDL2
	$(android) update project --path $(build)
	cd $(build) && $(ndk_build) $(NDKARGS)
	cd $(build) && $(ant) clean debug
	$(call echo,Test the app by specifying 'debug-install' as a recipe.)

game += $(game_debug_apk)

