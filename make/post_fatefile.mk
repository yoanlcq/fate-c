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

$(eval $(if $(target_windows32_msvc), targets += target_windows32_msvc ,,))
$(eval $(if $(target_windows64_msvc), targets += target_windows64_msvc ,,))
$(eval $(if $(target_windows32_gcc),  targets += target_windows32_gcc  ,,))
$(eval $(if $(target_windows64_gcc),  targets += target_windows64_gcc  ,,))
$(eval $(if $(target_windows32_clang),targets += target_windows32_clang,,))
$(eval $(if $(target_windows64_clang),targets += target_windows64_clang,,))
$(eval $(if $(target_linux32_gcc),    targets += target_linux32_gcc    ,,))
$(eval $(if $(target_linux64_gcc),    targets += target_linux32_gcc    ,,))
$(eval $(if $(target_linux32_clang),  targets += target_linux32_clang  ,,))
$(eval $(if $(target_linux64_clang),  targets += target_linux32_clang  ,,))
$(eval $(if $(target_osx_gcc),        targets += target_osx_clang      ,,))
$(eval $(if $(target_osx_clang),      targets += target_osx_clang      ,,))
$(eval $(if $(target_ios),            targets += target_ios            ,,))
$(eval $(if $(target_android),        targets += target_android        ,,))
$(eval $(if $(target_emscripten),     targets += target_emscripten     ,,))

sbuild=$(MAKE) -f $(fate)/single_build.mk

target_windows32_msvc :
	$(sbuild) os=windows arch=32 cc_id=cl
target_windows64_msvc :
	$(sbuild) os=windows arch=64 cc_id=cl
target_windows32_gcc  :
	$(sbuild) os=windows arch=32 cc_id=gcc
target_windows64_gcc  :
	$(sbuild) os=windows arch=64 cc_id=gcc
target_windows32_clang:
	$(sbuild) os=windows arch=32 cc_id=clang
target_windows64_clang:
	$(sbuild) os=windows arch=64 cc_id=clang
target_linux32_gcc    :
	$(sbuild) os=linux arch=32 cc_id=gcc
target_linux64_gcc    :
	$(sbuild) os=linux arch=64 cc_id=gcc
target_linux32_clang  :
	$(sbuild) os=linux arch=32 cc_id=clang
target_linux64_clang  :
	$(ensure $(linux64_clang))
	$(sbuild) os=linux arch=64 cc_id=clang
target_osx_gcc        :
	$(sbuild) os=osx cc_id=gcc
target_osx_clang      :
	$(sbuild) os=osx cc_id=clang
target_ios            :
	$(sbuild) os=ios cc_id=clang
target_android        :
	$(sbuild) os=android sdlpath=....
target_emscripten:
	$(if installed,emscripten_cc)
	@echo "" $(foreach b,$(active_builds), && $(sbuild) os=web cc_id=emcc build=$(b))

