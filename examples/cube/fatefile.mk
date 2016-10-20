dirs_src = src
dirs_include = src
dirs_gen = gen
dirs_art = art
dirs_obj = obj
dirs_res = res
dirs_bin = bin
dirs_dist = dist


include asset_pipeline.mk

# dist_debian = 
# dist_windows32 = 

# Build types :
# FE_MT_DISABLE
builds_mydebug_defines := MY_DEBUG_BUILD
builds_mydebug_debug := on
builds_mydebug_exename := cube_dbg
builds_myprofiled_defines := MY_PROFILED_BUILD
builds_myprofiled_cai := on
builds_myprofiled_vulkan := on
builds_myprofiled_d3d10 := on
builds_myprofiled_debug := on
builds_myprofiled_mingw_gui_app := on
builds_myprofiled_exename := cube_prof
builds_myrelease_defines := MY_NDEBUG
builds_myrelease_release := on
builds_myrelease_exename := cube
#active_builds := mydebug myrelease myprofiled
active_builds := mydebug

# target_windows32_cl    := yes
# target_windows64_cl    := yes
target_windows32_gcc   := yes
# target_windows64_gcc   := yes
# target_windows32_clang := yes
# target_windows64_clang := yes
# target_linux32_gcc     := yes
target_linux64_gcc     := yes
# target_linux32_clang   := yes
# target_linux64_clang   := yes
# target_osx_gcc         := yes
# target_osx_clang       := yes
# target_ios             := yes 
# target_android         := yes
# target_emscripten      := yes


# Custom var for later reuse.
my_name = Yoan Lecoq

game_emscripten_url = http://www.yoanlecoq.com/fate_cube_demo
game_emscripten_preload = $(dirs_res)/emscripten
game_emscripten_embed =

game_windows_company_name      = $(my_name)
game_windows_file_version      = 1.0
game_windows_file_description  = F.A.T.E Cube demo
game_windows_internal_name     = fate_cube_demo.exe
game_windows_legal_copyright   = $(my_name)
game_windows_legal_trademarks  =
game_windows_original_filename = fate_cube_demo.exe
game_windows_product_name      = F.A.T.E Cube demo
game_windows_product_version   = 1.0
game_windows_icon              = my_dir/cube.ico

game_android_reversed_domain = com.yoanlcq.cubedemo
game_android_activity_classname = CubeActivity
game_android_app_name = Cube Demo 2
game_android_app_ident = cube_demo
game_android_assets = $(dirs_res)/android
game_android_main = cube_demo_main
game_android_icon_48px  = my_dir/icon_48px.png
game_android_icon_72px  = my_dir/icon_72px.png
game_android_icon_96px  = my_dir/icon_96px.png
game_android_icon_144px = my_dir/icon_144px.png

game_ios_launch_screen_1080x1920 = 
game_ios_launch_screen_750x1334 = 
game_ios_launch_screen_640x1136 = 
game_ios_launch_screen_2048x2732 = 
game_ios_launch_screen_1536x2048 = 

game_ios_icon_58px =
game_ios_icon_80px =
game_ios_icon_87px =
game_ios_icon_120px =
game_ios_icon_152px =
game_ios_icon_167px =
game_ios_icon_180px =
game_ios_icon_1024px =

