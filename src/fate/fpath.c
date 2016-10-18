#include <fate/fate.h>

void fe_fpath_deinit(fe_fpath fpath) {
#if defined(FE_TARGET_EMSCRIPTEN)
    if(fpath.is_idb)
        fe_mem_heapfree(fpath.path), fe_mem_heapfree(fpath.db_name);
    else if(fpath.is_wget)
        fe_mem_heapfree(fpath.wget.url);
    else if(fpath.is_memfs)
        fe_mem_heapfree(fpath.memfs.path);
#else
    fe_mem_heapfree(fpath.path);
#endif
}



#if defined(FE_TARGET_EMSCRIPTEN)

fe_fpath fe_fpath_emscripten_idb(const char *db_name, const char *filepath) {
    fe_fpath fp = {0};
    fp.is_idb = true;
    fp.idb.path = fe_strdup(filepath);
    fp.idb.db_name = fe_strdup(fdb_name);
    return fp;
}
fe_fpath fe_fpath_emscripten_wget(const char *url) {
    fe_fpath fp = {0};
    fp.is_wget = true;
    fp.wget.url = fe_strdup(url);
    return fp;
}
fe_fpath fe_fpath_emscripten_memfs(const char *filepath) {
    fe_fpath fp = {0};
    fp.is_memfs = true;
    fp.memfs.path = fe_strdup(filepath);
    return fp;
}

#elif defined(FE_TARGET_OSX)

#include <unistd.h>

fe_fpath fe_fpath_osx_sdl2basepath(const char *filepath) {
    char *sdl2p = SDL_GetBasePath();
    char *fp = fe_asprintf("%s%s", sdl2p, filepath);
    SDL_free(sdl2p);
    return (fe_fpath){fp};
}
fe_fpath fe_fpath_osx_sdl2prefpath(const char *org, const char *app, const char *filepath) {
    char *sdl2p = SDL_GetPrefPath(org, app);
    char *fp = fe_asprintf("%s%s", sdl2p, filepath);
    SDL_free(sdl2p);
    return (fe_fpath){fp};
}
fe_fpath fe_fpath_osx_tmp(const char *filepath) {
    static char tmplate[] = "/tmp/tempXXXXXXXX";
    static const char *tmpdir = NULL;
    if(!tmpdir)
        fe_dbg_hope(tmpdir = mkdtemp(tmplate));
    return (fe_fpath){fe_asprintf("%s/%s", tmpdir, filepath)};
}

#elif defined(FE_TARGET_IOS)

fe_fpath fe_fpath_ios_sdl2basepath(const char *filepath) {
    char *sdl2p = SDL_GetBasePath();
    char *fp = fe_asprintf("%s%s", sdl2p, filepath);
    SDL_free(sdl2p);
    return (fe_fpath){fp};
}
fe_fpath fe_fpath_ios_sdl2prefpath(const char *org, const char *app, const char *filepath) {
    char *sdl2p = SDL_GetPrefPath(org, app);
    char *fp = fe_asprintf("%s%s", sdl2p, filepath);
    SDL_free(sdl2p);
    return (fe_fpath){fp};
}
fe_fpath fe_fpath_ios_tmp(const char *tmpdir, const char *filepath) {
    return (fe_fpath){fe_asprintf("%s/%s", tmpdir, filepath)};
}

#elif defined(FE_TARGET_ANDROID)

fe_fpath fe_fpath_android_external_storage(const char *filepath) {
    return (fe_fpath){
        fe_asprintf("%s/%s", SDL_AndroidGetExternalStoragePath(), filepath)
    };
}
fe_fpath fe_fpath_android_internal_storage(const char *not_assetdir, const char *filepath) {
    return (fe_fpath){
        fe_asprintf("%s/%s/%s", not_assetdir, 
                SDL_AndroidGetExternalStoragePath(), filepath)
    };
}
fe_fpath fe_fpath_android_apk_asset(const char *assetdir, const char *filepath) {
    return (fe_fpath){fe_asprintf("%s/%s", assetdir, filepath)};
}
fe_fpath fe_fpath_android_tmp(const char *tmpdir, const char *filepath) {
    return (fe_fpath){fe_asprintf("%s/%s", tmpdir, filepath)};
}

#elif defined(FE_TARGET_LINUX) || defined(FE_TARGET_FREEBSD)

fe_fpath fe_fpath_linux_executable_dir(const char *filepath) {
    /* TODO */
    fe_dbg_hope(false);
    return (fe_fpath){0};
}
fe_fpath fe_fpath_linux_xdg_data_home(const char *filepath) {
    /* XXX relying on the current SDL2 implementation 
     * Could be static, so as not to query the whole path every time. */
    char *sdl2p = SDL_GetPrefPath("", ".");
    char *fp = fe_asprintf("%s%s", sdl2p, filepath);
    SDL_free(sdl2p);
    return (fe_fpath){fp};
}
fe_fpath fe_fpath_linux_tmp_dir(const char *filepath) {
    static char tmplate[] = "/tmp/tempXXXXXXXX";
    static const char *tmpdir = NULL;
    if(!tmpdir)
        fe_dbg_hope(tmpdir = mkdtemp(tmplate));
    return (fe_fpath){fe_asprintf("%s/%s", tmpdir, filepath)};
}

#elif defined(FE_TARGET_WINDOWS_RT)

static fe_fpath winrtfpath(SDL_WinRT_Path p, const char *filepath) {
    return (fe_fpath){
        fe_asprintf("%s/%s", SDL_WinRTGetFSPathUTF8(p), filepath)
    };
}

fe_fpath fe_fpath_winrt_installed_location(const char *filepath) {
    return winrtfpath(SDL_WINRT_PATH_INSTALLED_LOCATION, filepath);
}
fe_fpath fe_fpath_winrt_local_folder(const char *filepath) {
    return winrtfpath(SDL_WINRT_PATH_LOCAL_FOLDER, filepath);
}
fe_fpath fe_fpath_winrt_roaming_folder(const char *filepath) {
    return winrtfpath(SDL_WINRT_PATH_ROAMING_FOLDER, filepath);
}
fe_fpath fe_fpath_winrt_temp_folder(const char *filepath) {
    return winrtfpath(SDL_WINRT_PATH_TEMP_FOLDER, filepath);
}

#elif defined(FE_TARGET_WINDOWS)

fe_fpath fe_fpath_windows_executable_folder(const char *filepath) {
    GetModuleFileNameW(NULL, buf, len);
}
fe_fpath fe_fpath_windows_appdata(const char *filepath) {
    !SUCCEEDED(SHGetKnownFolderPathW(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, path)
}
fe_fpath fe_fpath_windows_temp_folder(const char *filepath) {
    GetTempPathW(len, buf);
}

#endif


