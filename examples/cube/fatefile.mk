dirs_src = src
dirs_gen = gen
dirs_art = art
dirs_obj = obj
dirs_res = res
dirs_bin = bin
dirs_dist = dist

# dist_debian = 
# dist_windows32 = 

# Build types :
# FE_MT_DISABLE
# FE_DEBUG_BUILD
# FE_CAI_ENABLE
# FE_USE_VULKAN
builds_mydebug_defines := FE_DEBUG_BUILD
builds_mydebug_debug := on
builds_myprofiled_defines := FE_PROFILED_BUILD
builds_myprofiled_cai := on
builds_myprofiled_vulkan := on
builds_myprofiled_d3d10 := on
builds_myprofiled_debug := on
builds_myprofiled_mingw_gui_app := on
builds_myrelease_defines := NDEBUG
builds_myrelease_release := on
active_builds := mydebug myrelease myprofiled

target_windows32_msvc  =
target_windows64_msvc  =
target_windows32_gcc   =
target_windows64_gcc   =
target_windows32_clang =
target_windows64_clang =
target_linux32_gcc     =
target_linux64_gcc     =
target_linux32_clang   =
target_linux64_clang   =
target_osx_gcc         =
target_osx_clang       =
target_ios             = 
target_android         =
target_emscripten      =


# Custom var for later reuse.
my_name = Yoan Lecoq

game_emscripten_url = http://www.yoanlecoq.com/fate_cube_demo

game_windows_company_name = $(my_name)
game_windows_file_version = 1.0
game_windows_file_description = F.A.T.E Cube demo
game_windows_internal_name = fate_cube_demo.exe
game_windows_legal_copyright = $(my_name)
game_windows_legal_trademarks =
game_windows_original_filename = fate_cube_demo.exe
game_windows_product_name = F.A.T.E Cube demo
game_windows_product_version = 1.0
game_windows_icon = cube.ico

game_android_reversed_domain = com.yoanlcq.cube
game_android_activity_classname = CubeActivity
game_android_app_name = 
game_android_icon_48px = 
game_android_icon_72px = 
game_android_icon_96px = 
game_android_icon_144px = 

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

