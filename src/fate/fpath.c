#include <fate/fate.h>


/*
 *
 *
 *
 *
 *
 *
 *
 *    fe_fs_get_executable_dir()
 *
 *
 *
 *
 *
 *
 *
 */


#if defined(FE_TARGET_LINUX) && !defined(FE_TARGET_ANDROID)

#if !(BSD_SOURCE || _XOPEN_SOURCE >= 500  \
 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED || _POSIX_C_SOURCE >= 200112L)
#ifdef _GNU_SOURCE
#include <sys/syscall.h>
static inline ssize_t readlink(const char *path, char *buf, size_t bufsiz) {
    return syscall(SYS_readlink, path, buf, bufsiz);
}
static inline int lstat(const char *path, struct stat *buf) {
    return syscall(SYS_lstat, path, buf);
}
#else
#error Syscalls lstat() and readlink() are not available. Please update (g)libc or define _GNU_SOURCE.
#endif
#endif

#if __GLIBC__ > 2 || (__GLIBC__==2 && __GLIBC_MINOR__>=16)
#include <sys/auxv.h>
#endif
static char *get_executable_path(void) {
    struct stat st;
    char *str2, *str;
#if __GLIBC__ > 2 || (__GLIBC__==2 && __GLIBC_MINOR__>=16)
    str = (char*)getauxval(AT_EXECFN);
    if(str)
        return realpath(str, NULL);
#endif
    if(lstat("/proc/self/exe", &st) == 0) {
        str = fe_mem_heapalloc(st.st_size+1, char, "");
        if(readlink("/proc/self/exe", str, st.st_size) > 0) {
            str[st.st_size] = '\0';
            str2 = realpath(str, NULL);
            fe_mem_heapfree(str);
            return str2;
        }
        fe_mem_heapfree(str);
    }
    return NULL;
}

#elif defined(FE_TARGET_FREEBSD)

static char *get_executable_path(void) {
    static char appdir[PATH_MAX];
    char procfs[2][19] = {
        "/proc/curproc/exe",
        "/proc/curproc/file"
    };
    int i, mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
    size_t size = PATH_MAX;
    struct stat st;
    char *str, *str2;
    for(i=0 ; i<2 ; ++i) {
        if(lstat(procfs[i], &st) == 0) {
            str = fe_mem_heapalloc(st.st_size+1, char, "");
            if(readlink(procfs[i], str, st.st_size) > 0) {
                str[st.st_size] = '\0';
                str2 = realpath(str, NULL);
                fe_mem_heapfree(str);
                return str2;
            }
            fe_mem_heapfree(str);
        }
    }
    if(sysctl(mib, 4, appdir, &size, NULL, 0) == 0)
        return realpath(appdir, NULL);
    return NULL;
}


#elif defined(FE_TARGET_OSX) || defined(FE_TARGET_IOS)

static char *get_executable_path(void) {
    static char appdir[MAXPATHLEN];
    char *str, *str2;
    uint32_t size = MAXPATHLEN;
    if(_NSGetExecutablePath(appdir, &size) == 0)
        return realpath(appdir, NULL);
    str = fe_mem_heapalloc(size, char, "");
    _NSGetExecutablePath(str, &size);
    str2 = realpath(str, NULL);
    fe_mem_heapfree(str);
    return str2;
}

#elif defined(FE_TARGET_WINDOWS)
/*
 * This one gets us an undefined reference on MinGW.
static char *get_executable_path(void) {
    wchar_t *wpath;
    fe_dbg_hope(!_get_wpgmptr(&wpath));
    return fe_utf8_from_win32unicode(wpath);
}
*/
static char *get_executable_path(void) {
    WCHAR *buf = NULL;
    size_t len = 0;
    do {
        len += 256;
        buf = fe_mem_heaprealloc(buf, len, WCHAR, "");
        fe_dbg_hope(GetModuleFileNameW(NULL, buf, len));
    } while(GetLastError() == ERROR_INSUFFICIENT_BUFFER);
    char *utf8 = fe_utf8_from_win32unicode(buf);
    fe_mem_heapfree(buf);
    return utf8;
}
#endif

#ifndef FE_TARGET_EMSCRIPTEN

#if defined(FE_TARGET_WINDOWS)
    #define PATHSEP "\\"
#else
    #define PATHSEP "/"
#endif

static bool remove_last_path_component(char *path) {
    char *ind = strrchr(path, *PATHSEP);
    if(*ind) {
        *ind = '\0';
        return true;
    }
    return false;
}

static char *get_executable_dir(void) {
    char *expath = get_executable_path();
    if(!expath)
        return NULL;
    remove_last_path_component(expath);
    return expath;
}

#undef PATHSEP
#endif /* EMSCRIPTEN */












void fe_fpath_deinit(fe_fpath fpath) {
#if defined(FE_TARGET_EMSCRIPTEN)
    if(fpath.is_idb)
        fe_mem_heapfree(fpath.idb.path), fe_mem_heapfree(fpath.idb.db_name);
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
    fe_fpath fp = FE_FPATH_ZERO;
    fp.is_idb = true;
    fp.idb.path = fe_strdup(filepath);
    fp.idb.db_name = fe_strdup(db_name);
    return fp;
}
fe_fpath fe_fpath_emscripten_wget(const char *url) {
    fe_fpath fp = FE_FPATH_ZERO;
    fp.is_wget = true;
    fp.wget.url = fe_strdup(url);
    return fp;
}
fe_fpath fe_fpath_emscripten_memfs(const char *filepath) {
    fe_fpath fp = FE_FPATH_ZERO;
    fp.is_memfs = true;
    fp.memfs.path = fe_asprintf("/memfs/%s", filepath);
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
fe_fpath fe_fpath_osx_executable_dir(const char *filepath) {
    /* XXX We shouldn't compute it every time ? */
    char *exedir = get_executable_dir();
    fe_fpath ret = {fe_asprintf("%s/%s", exedir, filepath)};
    fe_mem_heapfree(exedir);
    return ret;
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
    /* XXX We shouldn't compute it every time ? */
    char *exedir = get_executable_dir();
    fe_fpath ret = {fe_asprintf("%s/%s", exedir, filepath)};
    fe_mem_heapfree(exedir);
    return ret;
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
    /* XXX We shouldn't compute it every time ? */
    char *exedir = get_executable_dir();
    fe_fpath ret = {fe_asprintf("%s\\%s", exedir, filepath)};
    fe_mem_heapfree(exedir);
    return ret;
}
fe_fpath fe_fpath_windows_roaming_appdata(const char *filepath) {
    /* XXX We shouldn't compute it every time ? */
    WCHAR *wpath;
    KNOWNFOLDERID kfid = FOLDERID_RoamingAppData;
    HRESULT res = SHGetKnownFolderPath(
        &kfid, KF_FLAG_INIT | KF_FLAG_CREATE, NULL, &wpath
    );
    fe_dbg_hope(SUCCEEDED(res));
    char *utf8 = fe_utf8_from_win32unicode(wpath);
    CoTaskMemFree(wpath);
    fe_fpath fpath = { fe_asprintf("%s/%s", utf8, filepath) };
    fe_mem_heapfree(utf8);
    return fpath;
}
fe_fpath fe_fpath_windows_temp_folder(const char *filepath) {
    /* XXX We shouldn't compute it every time ? */
    WCHAR *wpath = NULL;
    size_t nlen, len = GetTempPathW(0, NULL);
    for(;;) {
        wpath = fe_mem_heaprealloc(wpath, len, WCHAR, "");
        nlen = GetTempPathW(len, wpath);
        if(nlen <= len)
            break;
        len = nlen;
    }
    char *utf8 = fe_utf8_from_win32unicode(wpath);
    fe_mem_heapfree(wpath);
    fe_fpath fpath = { fe_asprintf("%s/%s", utf8, filepath) };
    fe_mem_heapfree(utf8);
    return fpath;
}

#endif

