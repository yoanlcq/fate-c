# Pour chaque target _demandée_...
#     Si la target est supportée...
#         Pour chaque build _demandée_...
#             Faire une invocation de make

.PHONY: target_default         \
        target_windows32_msvc  \
        target_windows64_msvc  \
        target_windows32_gcc   \
        target_windows64_gcc   \
        target_windows32_clang \
        target_windows64_clang \
        target_linux32_gcc     \
        target_linux64_gcc     \
        target_linux32_clang   \
        target_linux64_clang   \
        target_osx_gcc         \
        target_osx_clang       \
        target_ios             \
        target_android         \
        target_emscripten      

$(eval $(if $(target_windows32_msvc), targets += target_windows32_msvc ,))
$(eval $(if $(target_windows64_msvc), targets += target_windows64_msvc ,))
$(eval $(if $(target_windows32_gcc),  targets += target_windows32_gcc  ,))
$(eval $(if $(target_windows64_gcc),  targets += target_windows64_gcc  ,))
$(eval $(if $(target_windows32_clang),targets += target_windows32_clang,))
$(eval $(if $(target_windows64_clang),targets += target_windows64_clang,))
$(eval $(if $(target_linux32_gcc),    targets += target_linux32_gcc    ,))
$(eval $(if $(target_linux64_gcc),    targets += target_linux64_gcc    ,))
$(eval $(if $(target_linux32_clang),  targets += target_linux32_clang  ,))
$(eval $(if $(target_linux64_clang),  targets += target_linux64_clang  ,))
$(eval $(if $(target_osx_gcc),        targets += target_osx_gcc        ,))
$(eval $(if $(target_osx_clang),      targets += target_osx_clang      ,))
$(eval $(if $(target_ios),            targets += target_ios            ,))
$(eval $(if $(target_android),        targets += target_android        ,))
$(eval $(if $(target_emscripten),     targets += target_emscripten     ,))

targets:=$(strip $(targets))

sbuild=$(MAKE) $(MAKEFLAGS) -f $(fate)/make/single_build.mk see_all_cmd=yes
tgt_warn=$(warning Will not build for $(1) (not installed).)
build_this=$(strip \
		$(if $($(1)_available), \
			echo "Running with --dry-run..." $(foreach b,$(active_builds), && $(sbuild) $(2) build=$(b)), \
		 	$(call tgt_warn,$(1)) \
		) \
	)

target_windows32_msvc :
	$(call build_this,$@,os=windows arch=32 cc_id=cl)
target_windows64_msvc :
	$(call build_this,$@,os=windows arch=64 cc_id=cl)
target_windows32_gcc  :
	$(call build_this,$@,os=windows arch=32 cc_id=gcc)
target_windows64_gcc  :
	$(call build_this,$@,os=windows arch=64 cc_id=gcc)
target_windows32_clang:
	$(call build_this,$@,os=windows arch=32 cc_id=clang)
target_windows64_clang:
	$(call build_this,$@,os=windows arch=64 cc_id=clang)
target_linux32_gcc    :
	$(call build_this,$@,os=linux arch=32 cc_id=gcc)
target_linux64_gcc    :
	$(call build_this,$@,os=linux arch=64 cc_id=gcc)
target_linux32_clang  :
	$(call build_this,$@,os=linux arch=32 cc_id=clang)
target_linux64_clang  :
	$(call build_this,$@,os=linux arch=64 cc_id=clang)
target_osx_gcc        :
	$(call build_this,$@,os=osx cc_id=gcc)
target_osx_clang      :
	$(call build_this,$@,os=osx cc_id=clang)
target_ios            :
	$(call build_this,$@,os=ios cc_id=clang)
target_android        :
	$(call build_this,$@,os=android sdlpath=....)
target_emscripten     :
	$(call build_this,$@,os=web cc_id=emcc)

