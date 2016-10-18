#include <fate/fate.h>
#include "res.h"

const res_list res = {
    .cubemaps.flame.bk = "flame_bk.jpg",
    .cubemaps.flame.dn = "flame_dn.jpg",
    .cubemaps.flame.ft = "flame_ft.jpg",
    .cubemaps.flame.lf = "flame_lf.jpg",
    .cubemaps.flame.rt = "flame_rt.jpg",
    .cubemaps.flame.up = "flame_up.jpg",
    .musics.deeper     = "Deeper.ogg"
};

fe_fpath res_path(const char *filepath) {
#if defined(FE_TARGET_EMSCRIPTEN)
    return fe_fpath_emscripten_memfs(filepath);
#elif defined(FE_TARGET_OSX)
    return fe_fpath_osx_sdl2basepath(filepath);
#elif defined(FE_TARGET_IOS)
    return fe_fpath_ios_sdl2basepath(filepath);
#elif defined(FE_TARGET_ANDROID)
    fe_fpath_android_apk_asset("assets", filepath);
#elif defined(FE_TARGET_LINUX) || defined(FE_TARGET_FREEBSD)
    return fe_fpath_linux_executable_dir("../res/" filepath);
#elif defined(FE_TARGET_WINDOWS_RT)
#error "Windows RT not supported."
#elif defined(FE_TARGET_WINDOWS)
    return fe_fpath_windows_executable_folder("../res/" filepath);
#endif
}

bool res_load(fe_iov *iov, const char *filepath) {
    fe_fpath fpath = res_path(filepath);
    bool ret = fe_fs_load(iov, fpath);
#ifndef FE_TARGET_EMSCRIPTEN
    if(!ret)
        fe_fatal("Could not load res `%s' !\n", fpath.path);
#endif
    fe_fpath_deinit(fpath);
    return ret;
}
