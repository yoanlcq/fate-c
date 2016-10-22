#include <fate/fate.h>
#include "res.h"

const res_list res = {
    .cubemaps.flame.bk.jpg = "flame_bk.jpg",
    .cubemaps.flame.dn.jpg = "flame_dn.jpg",
    .cubemaps.flame.ft.jpg = "flame_ft.jpg",
    .cubemaps.flame.lf.jpg = "flame_lf.jpg",
    .cubemaps.flame.rt.jpg = "flame_rt.jpg",
    .cubemaps.flame.up.jpg = "flame_up.jpg",
    .musics.deeper.ogg     = "Deeper.ogg",
    .musics.wind.ogg = "144083__bosk1__wind-houling-1.ogg"
};

fe_fpath res_path(const char *filepath) {
#if defined(FE_TARGET_EMSCRIPTEN)
    char *npath = fe_asprintf("res/%s", filepath);
    fe_fpath fpath = fe_fpath_emscripten_memfs(npath);
    fe_mem_heapfree(npath);
    return fpath;
#elif defined(FE_TARGET_OSX)
    return fe_fpath_osx_sdl2basepath(filepath);
#elif defined(FE_TARGET_IOS)
    return fe_fpath_ios_sdl2basepath(filepath);
#elif defined(FE_TARGET_ANDROID)
    return fe_fpath_android_apk_asset("res", filepath);
#elif defined(FE_TARGET_LINUX) || defined(FE_TARGET_FREEBSD)
    char *pth = fe_asprintf("../../res/%s", filepath);
    fe_fpath ret = fe_fpath_linux_executable_dir(pth);
    fe_mem_heapfree(pth);
    return ret;
#elif defined(FE_TARGET_WINDOWS_RT)
#error "Windows RT not supported."
#elif defined(FE_TARGET_WINDOWS)
    char *pth = fe_asprintf("..\\..\\res\\%s", filepath);
    fe_fpath ret = fe_fpath_windows_executable_folder(pth);
    fe_mem_heapfree(pth);
    return ret;
#endif
}
static const char *TAG = "res";
bool res_load(fe_iov *iov, const char *filepath) {
    fe_dbg_assert(!iov->base && !iov->len);
    fe_fpath fpath = res_path(filepath);
#ifdef FE_TARGET_EMSCRIPTEN
    fe_logi(TAG, "Loading `%s'...\n", fpath.memfs.path);
#else
    fe_logi(TAG, "Loading `%s'...\n", fpath.path);
#endif
    bool ret = fe_fs_load(iov, fpath);
    if(!ret) {
#ifdef FE_TARGET_EMSCRIPTEN
        fe_fatal(TAG, "Could not load res `%s' !\n", fpath.memfs.path);
#else
        fe_fatal(TAG, "Could not load res `%s' !\n", fpath.path);
#endif
    }
    fe_fpath_deinit(fpath);
    return ret;
}
