cflags= -std=c11 -g -DFE_DEBUG_BUILD -DFE_LINUXPERF_UNSUPPORTED
gm_rdomain:=$(game_android_reversed_domain)
gm_rdomain_dir:=$(subst .,/,$(gm_rdomain))
gm_cfiles:=\$$(call rglob,\$$(LOCAL_PATH),*.c)
gm_build_dir:=$(dirs_obj)/android
gm:=$(game_android_app_ident)
gm_activity:=$(game_android_activity_classname)

abase:=$(fate)/dist/android_base

$(gm_build_dir)/.copied:
	@$(call mkdir_p,$(@D))
	$(call cp_r,$(abase)/*,$(@D))
	echo > $@

$(gm_build_dir)/jni/src/Android.mk: $(abase)/m4/Android.mk.m4
	@$(call mkdir_p,$(@D))
	$(gnum4) -DGAME="$(gm)" -DGAME_CFLAGS="$(cflags)" -DGAME_SOURCE_FILES="$(gm_cfiles)" $< > $@
$(gm_build_dir)/jni/src/main.c: $(abase)/m4/main.c.m4
	@$(call mkdir_p,$(@D))
	$(gnum4) -DGAME_MAIN="$(game_android_main)" $< > $@
$(gm_build_dir)/AndroidManifest.xml: $(abase)/m4/AndroidManifest.xml.m4
	@$(call mkdir_p,$(@D))
	$(gnum4) -DGAME_APP_DOMAIN="$(game_android_reversed_domain)" -DGAME_ACTIVITY=$(gm_activity) $< > $@
$(gm_build_dir)/build.xml: $(abase)/m4/build.xml.m4
	@$(call mkdir_p,$(@D))
	$(gnum4) -DGAME_APP_DOMAIN="$(game_android_reversed_domain)" $< > $@
$(gm_build_dir)/res/values/strings.xml: $(abase)/m4/strings.xml.m4
	@$(call mkdir_p,$(@D))
	$(gnum4) -DGAME_APP_NAME="$(game_android_app_name)" $< > $@
$(gm_build_dir)/src/$(gm_rdomain_dir)/$(gm_activity).java: $(abase)/m4/MyActivity.java.m4
	@$(call mkdir_p,$(@D))
	$(gnum4) -DGAME_APP_DOMAIN="$(game_android_reversed_domain)" -DGAME_ACTIVITY="$(gm_activity)" $< > $@

$(gm_build_dir)/res/drawable-mdpi/ic_launcher.png: $(game_android_icon_48px)
	@$(call mkdir_p,$(@D))
	$(call cp_r,$<,$@)
$(gm_build_dir)/res/drawable-hdpi/ic_launcher.png: $(game_android_icon_72px)
	@$(call mkdir_p,$(@D))
	$(call cp_r,$<,$@)
$(gm_build_dir)/res/drawable-xhdpi/ic_launcher.png: $(game_android_icon_96px)
	@$(call mkdir_p,$(@D))
	$(call cp_r,$<,$@)
$(gm_build_dir)/res/drawable-xxhdpi/ic_launcher.png: $(game_android_icon_144px)
	@$(call mkdir_p,$(@D))
	$(call cp_r,$<,$@)

$(gm_build_dir)/jni/SDL/src:
	@$(call mkdir_p,$(@D))
	$(call ln_sf,$(sdl2)/$(@F),$@)
$(gm_build_dir)/jni/SDL/include:
	@$(call mkdir_p,$(@D))
	$(call ln_sf,$(sdl2)/$(@F),$@)
$(gm_build_dir)/jni/SDL/Android.mk:
	@$(call mkdir_p,$(@D))
	$(call ln_sf,$(sdl2)/$(@F),$@)

$(gm_build_dir)/jni/src/fate:
	@$(call mkdir_p,$(@D))
	$(call ln_sf,$(fate)/src/fate,$@)
$(gm_build_dir)/jni/src/$(gm):
	@$(call mkdir_p,$(@D))
	$(call ln_sf,$(abspath $(dirs_src)),$@)
$(gm_build_dir)/jni/src/contrib:
	@$(call mkdir_p,$(@D))
	$(call ln_sf,$(fate)/src/contrib,$@)
$(gm_build_dir)/jni/include/fate:
	@$(call mkdir_p,$(@D))
	$(call ln_sf,$(fate)/include/fate,$@)
$(gm_build_dir)/jni/include/$(gm):
	@$(call mkdir_p,$(@D))
	$(call ln_sf,$(abspath $(dirs_include)),$@)
$(gm_build_dir)/jni/include/contrib:
	@$(call mkdir_p,$(@D))
	$(call ln_sf,$(fate)/include/contrib,$@)
$(gm_build_dir)/jni/include/SDL2:
	@$(call mkdir_p,$(@D))
	$(call ln_sf,../SDL/include,$@)
$(gm_build_dir)/assets:
	$(call ln_sf,$(game_android_assets),$@)


game_debug_apk=$(gm_build_dir)/bin/$(game_android_reversed_domain)-debug.apk

ifeq ($(ndk_j),)
ndk_j:=1
endif
ndk_args:= -j$(ndk_j)

$(game_debug_apk): $(gm_build_dir)/.copied \
				   $(gm_build_dir)/jni/src/Android.mk \
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
				   | \
				   $(gm_build_dir)/jni/SDL/src \
				   $(gm_build_dir)/jni/SDL/include \
				   $(gm_build_dir)/jni/src/fate \
				   $(gm_build_dir)/jni/src/$(gm) \
				   $(gm_build_dir)/jni/src/contrib \
				   $(gm_build_dir)/jni/include/fate \
				   $(gm_build_dir)/jni/include/$(gm) \
				   $(gm_build_dir)/jni/include/contrib \
				   $(gm_build_dir)/jni/include/SDL2 \
				   $(gm_build_dir)/assets
	@$(call mkdir_p,$(@D))
	$(android) update project --path $(gm_build_dir)
	cd $(gm_build_dir) && $(ndk_build) $(ndk_args)
	cd $(gm_build_dir) && $(ant) clean debug
	cd $(gm_build_dir) && $(ant) debug install
# $(call echo,Test the app by specifying 'debug-install' as a recipe.)

game:=$(game_debug_apk)

