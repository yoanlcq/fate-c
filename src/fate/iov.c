#include <fate/defs.h>
#include <fate/iov.h>
#include <fate/mem.h>
#include <fate/utf8.h>
#ifdef FE_TARGET_EMSCRIPTEN
#include <emscripten.h>
#endif
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdarg.h>
#include <fate/dbg.h>
#include <string.h>


#if defined(FE_TARGET_WINDOWS)
#include <Windows.h>
#elif defined(FE_TARGET_LINUX)
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#elif defined(FE_TARGET_FREEBSD)
#include <limits.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#elif defined(FE_TARGET_OSX) || defined(FE_TARGET_IOS)
#include <stdint.h>
#include <limits.h>
#include <signal.h>
#include <mach-o/dyld.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <errno.h>
#endif



static const char *TAG = "fe_iov";


#if defined(FE_TARGET_LINUX) && !defined(FE_TARGET_ANDROID)
#define NUL_FILE_PATH "/dev/null"
#elif defined(FE_TARGET_WINDOWS)
#define NUL_FILE_PATH "nul"
#endif
#define NUL_FILE_FALLBACK_NAME ".nul"
static FILE *nul_file = NULL;
static const char *nul_file_path = "";

void fe_iov_setup(void) {
#ifdef NUL_FILE_PATH
    nul_file_path = NUL_FILE_PATH;
#elif defined(FE_TARGET_ANDROID)
    static const char *nul_name = "/" NUL_FILE_FALLBACK_NAME;
    const size_t nul_name_len = strlen(nul_name);
    const char *internal = SDL_AndroidGetInternalStoragePath();
    const size_t internal_len = strlen(internal);
    nul_file_path = fe_mem_heapalloc(nul_name_len+internal_len+1, char, "");
    fe_dbg_hope(nul_file_path);
    strcpy(nul_file_path, internal);
    strcat(nul_file_path, nul_name);

    /* XXX might want to get more rigourous about this later. */
    mkdir(internal, 0755);
    /*
    struct stat sb;
    int status = stat(internal, &sb);
    if(status || (!status && !S_ISDIR(sb.st_mode))) {
        unlink(internal);
        ...
    }
    */
#else
    nul_file_path = fe_mem_heapalloc(strlen(NUL_FILE_FALLBACK_NAME)+1, char, "");
    fe_dbg_hope(nul_file_path);
    strcpy(nul_file_path, NUL_FILE_FALLBACK_NAME);
#endif
    fe_logi(TAG, "Attempting to open `%s' as the null file "
                 "for fe_iov_get_vprintf_len().\n", nul_file_path);
    nul_file = fopen(nul_file_path, "wb");
    fe_dbg_hope(nul_file);
}
void fe_iov_cleanup(void) {
    fclose(nul_file);
#ifndef NUL_FILE_PATH
    unlink(nul_file_path);
    fe_mem_heapfree(nul_file_path);
#endif
}

size_t  fe_iov_get_vprintf_len(const char *fmt, va_list ap) {
    int cnt = vfprintf(nul_file, fmt, ap);
    fe_dbg_assert(cnt >= 0);
    fseek(nul_file, 0, SEEK_SET);
    return cnt;
}
size_t  fe_iov_get_printf_len(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int cnt = fe_iov_get_vprintf_len(fmt, ap);
    va_end(ap);
    return cnt;
}

void    fe_iov_deinit(fe_iov *iov) {
    fe_mem_heapfree(iov->base);
}
bool    fe_iov_resize(fe_iov *iov, size_t len) {
    iov->base = fe_mem_heaprealloc(iov->base, len+1, char, "");
    iov->base[len] = '\0';
    iov->len  = len;
    return !!(iov->base);
}
size_t     fe_iov_printf(fe_iov *iov, size_t offset, const char *fmt, ...) {
    
    va_list ap;
    
    va_start(ap, fmt);
    size_t len = fe_iov_get_vprintf_len(fmt, ap);
    va_end(ap);

    fe_iov_resize(iov, iov->len+len);
    
    va_start(ap, fmt);
    vsprintf(iov->base+offset, fmt, ap);
    va_end(ap);
    
    return len;
}

void    fe_iov_copy(fe_iov *iov, size_t offset, const fe_iov *src) {
    if(offset + src->len > iov->len)
        fe_iov_resize(iov, offset + src->len);
    memcpy(iov->base + offset, src->base, src->len);
}






fe_iov_status  fe_iov_load_wget(fe_iov *iov, const fe_iov_locator *params) {
#ifdef FE_TARGET_EMSCRIPTEN
    int len, error;
    emscripten_wget_data(params->url, &iov->base, &len, &error);
    iov->len = len;
    fe_iov_status status = {0};
    status.step = FE_IOV_STEP_COMPLETED;
    status.success = !error;
    status.current = error ? FE_IOV_FAILED_UNKNOWN : FE_IOV_SUCCESS;
    return status;
#else
    /* TODO */
    fe_dbg_assert(0 && "This has not yet been implemented!!");
    fe_iov_status status = {0}; return status;
#endif
}
fe_iov_promise fe_iov_load_wget_async(fe_iov *iov, const fe_iov_locator *params) {return NULL;}

void static_fullpath(fe_iov *fullpath, const fe_iov_locator *params) {
#define HELPER(cst, call) \
    if(params->rootdir & cst) { \
        fe_iov_printf(fullpath, 0, "%s/%s", call, params->file_name); \
        return; \
    }
    HELPER(FE_IOV_ROOTDIR_SDL2_GETPREFPATH, SDL_GetPrefPath(params->organization, params->app_name))
    HELPER(FE_IOV_ROOTDIR_SDL2_GETBASEPATH, SDL_GetBasePath())
#ifdef FE_TARGET_ANDROID
    HELPER(FE_IOV_ROOTDIR_ANDROID_INTERNAL_STORAGE, SDL_AndroidGetInternalStoragePath())
    HELPER(FE_IOV_ROOTDIR_ANDROID_EXTERNAL_STORAGE, SDL_AndroidGetExternalStoragePath())
#elif defined(FE_TARGET_WINDOWS_RT)
    HELPER(FE_IOV_ROOTDIR_WINRT_PATH_LOCAL_FOLDER, SDL_WinRTGetFSPathUTF8(SDL_WINRT_PATH_LOCAL_FOLDER))
    HELPER(FE_IOV_ROOTDIR_WINRT_PATH_INSTALLED_LOCATION, SDL_WinRTGetFSPathUTF8(SDL_WINRT_PATH_INSTALLED_LOCATION))
#endif
#undef HELPER
}

fe_iov_status  fe_iov_load_file(fe_iov *iov, const fe_iov_locator *params) {
    fe_iov_status status = {0};
    status.step = FE_IOV_STEP_COMPLETED;
    fe_fd fd = fe_fd_open_file(params);
    if(!fe_fd_is_valid(fd)) {
        status.current = FE_IOV_FAILED_UNKNOWN;
        return status;
    }
    size_t len = fe_fd_seek(fd, FE_FD_SEEK_END, 0);
    fe_fd_seek(fd, FE_FD_SEEK_SET, 0);
    if(!fe_iov_resize(iov, len)) {
        status.current = FE_IOV_FAILED_NO_MEM;
        return status;
    }
    fe_fd_read(fd, iov->base, len);
    fe_fd_close(fd);
    status.success = true;
    return status;
}
fe_iov_promise fe_iov_load_file_async(fe_iov *iov, const fe_iov_locator *params) {return NULL;}
fe_iov_status  fe_iov_load_persistent(fe_iov *iov, const fe_iov_locator *params) {
#ifdef FE_TARGET_EMSCRIPTEN
    int len, error;
    emscripten_idb_load(params->idb_name, params->file_name, &iov->base, &len, &error);
    iov->len = len;
    fe_iov_status status = {0};
    status.step = FE_IOV_STEP_COMPLETED;
    status.success = !error;
    status.current = error ? FE_IOV_FAILED_UNKNOWN : FE_IOV_SUCCESS;
    return status;
#else
    return fe_iov_load_file(iov, params);
#endif
}
fe_iov_promise fe_iov_load_persistent_async(fe_iov *iov, const fe_iov_locator *params) {return NULL;}
fe_iov_status  fe_iov_load_res(fe_iov *iov, const fe_iov_locator *params) {
#ifdef FE_TARGET_EMSCRIPTEN
    return fe_iov_load_wget(iov, params);
#else
    return fe_iov_load_file(iov, params);
#endif
}
fe_iov_promise fe_iov_load_res_async(fe_iov *iov, const fe_iov_locator *params) {return NULL;}
fe_iov_status  fe_iov_store_file(fe_iov *iov, const fe_iov_locator *params) {
    fe_iov_status status = {0};
    status.step = FE_IOV_STEP_COMPLETED;
    fe_fd fd = fe_fd_open_file(params);
    if(!fe_fd_is_valid(fd)) {
        status.current = FE_IOV_FAILED_UNKNOWN;
        return status;
    }
    fe_fd_write(fd, iov->base, iov->len);
    fe_fd_close(fd);
    status.success = true;
    return status;
}
fe_iov_promise fe_iov_store_file_async(fe_iov *iov, const fe_iov_locator *params) {return NULL;}
fe_iov_status  fe_iov_store_persistent(fe_iov *iov, const fe_iov_locator *params) {
#ifdef FE_TARGET_EMSCRIPTEN
    int error;
    emscripten_idb_store(params->idb_name, params->file_name, iov->base, iov->len, &error);
    fe_iov_status status = {0};
    status.step = FE_IOV_STEP_COMPLETED;
    status.success = !error;
    status.current = error ? FE_IOV_FAILED_UNKNOWN : FE_IOV_SUCCESS;
    return status;
#else
    return fe_iov_store_file(iov, params);
#endif

}
fe_iov_promise fe_iov_store_persistent_async(fe_iov *iov, const fe_iov_locator *params) {return NULL;}


fe_fd          fe_fd_open_file(const fe_iov_locator *params) {return (fe_fd)-1;}
fe_fd          fe_fd_open_persistent(const fe_iov_locator *params) {return (fe_fd)-1;}
fe_fd          fe_fd_open_res(const fe_iov_locator *params) {return (fe_fd)-1;}
fe_iov_promise fe_fd_get_download_promise(fe_fd fd) {return NULL;}
bool           fe_fd_is_valid(fe_fd fd) {return 0;}
void*          fe_fd_mmap(fe_fd fd, fe_fd_offset offset, size_t len, bool rw) {return NULL;}
void           fe_fd_munmap(void *addr, size_t len) {}
fe_fd_offset   fe_fd_seek(fe_fd fd, fe_fd_offset offset, fe_fd_seek_whence whence) {return 0;}
ssize_t        fe_fd_read(fe_fd fd, void *buf, size_t len) {return 0;}
ssize_t        fe_fd_write(fe_fd fd, const void *buf, size_t len) {return 0;}
ssize_t        fe_fd_readv(fe_fd fd, fe_iov *iov_array, size_t iov_count) {return 0;}
ssize_t        fe_fd_writev(fe_fd fd, const fe_iov *iov_array, size_t iov_count) {return 0;}
ssize_t        fe_fd_preadv(fe_fd fd, fe_iov *iov_array, size_t iov_count, fe_fd_offset offset) {return 0;}
ssize_t        fe_fd_pwritev(fe_fd fd, const fe_iov *iov_array, size_t iov_count, fe_fd_offset offset) {return 0;}
bool           fe_fd_sync(fe_fd fd) {return 0;}
bool           fe_fd_truncate(fe_fd fd, size_t len) {return 0;}
void           fe_fd_close(fe_fd fd) {}

bool fe_iov_promise_poll(fe_iov_promise p, fe_iov_state *st) {return 0;}
bool fe_iov_promise_wait(fe_iov_promise p, fe_iov_state *st, int timeout_milliseconds) {return 0;}
void fe_iov_promise_cancel(fe_iov_promise p) {}









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


#if defined(FE_TARGET_EMSCRIPTEN)
char *fe_fs_get_executable_dir(void) {
    return ""; /* FIXME */
}
#elif defined(FE_TARGET_LINUX)

#ifndef FE_TARGET_ANDROID
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
#endif /* !FE_TARGET_ANDROID */

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
static char *get_executable_path(void) {
    return strdup(_pgmptr);
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

char *fe_fs_get_executable_dir(void) {
    char *expath = get_executable_path();
    if(!expath)
        return NULL;
    size_t size = strlen(expath);
    char *res  = fe_mem_heapalloc(size+strlen(PATHSEP"res")+1, char, "");
    char *data = fe_mem_heapalloc(size+strlen(PATHSEP"data")+1, char, "");

    int i;
    for(i=0 ; i<3 ; ++i) {
        if(!remove_last_path_component(expath))
            break;
        strcpy(res,  expath);
        strcat(res,  PATHSEP"res");
        strcpy(data, expath);
        strcat(data, PATHSEP"data");
        /* FIXME!!
        if(fe_fs_exists(res) && fe_fs_exists(data)) {
            fe_mem_heapfree(res);
            fe_mem_heapfree(data);
            return expath;
        }
        */
    }
    fe_mem_heapfree(res);
    fe_mem_heapfree(data);
    fe_loge(TAG, "Could not find res/ and data/ directories.\n");
    return NULL;
}

#undef PATHSEP
#endif /* EMSCRIPTEN */



bool fe_fs_setcwd(const fe_iov_locator *params) {
    fe_iov fullpath = {0};
    static_fullpath(&fullpath, params);
#if defined(FE_TARGET_EMSCRIPTEN)
    bool ret = false; /* TODO */
#elif defined(FE_TARGET_WINDOWS)
    bool ret = SetCurrentDirectory(fullpath.base);
#else
    bool ret = !chdir(fullpath.base);
#endif
    fe_mem_heapfree(fullpath.base);
    return ret;
}

/*
char * fe_fs_getcwd(void) {
#if defined(FE_TARGET_EMSCRIPTEN)
    int size = 2048;
    char *path = fe_mem_heapalloc(size, char, "fe_fs_getcwd()");
    if(!path)
        return NULL;
    EM_ASM({
        Module.stringToUTF8(FS.cwd(), $0, $1);
    }, path, size);
    return path;
#elif defined(FE_TARGET_WINDOWS)
    DWORD len = GetCurrentDirectoryW(0, NULL);
    WCHAR *wpath = fe_mem_heapalloc(len, WCHAR, "");
    if(!wpath)
        return NULL;
    GetCurrentDirectoryW(len, wpath);
    const char *path = fe_utf8_from_win32unicode(wpath);
    if(!path)
        return NULL;
    fe_mem_heapfree(wpath);
    return path;
#else
    size_t size = 256;
    char *path = fe_mem_heapalloc(size, char, "fe_fs_getcwd()");
    if(!path)
        return NULL;
    while(!getcwd(path, size) && errno==ERANGE) {
        size *= 2;
        path = fe_mem_heaprealloc(path, size, char, "fe_fs_getcwd()");
        if(!path)
            return NULL;
    }
    return path;
}

uint64_t fe_fs_file_get_wtime(const char *path) {
#if defined(FE_TARGET_EMSCRIPTEN)
    return EM_ASM_INT({
        var path = Module.UTF8ToString($0);
        return FS.stat(path).mtime;
    }, params->file_name);
#elif defined(FE_TARGET_WINDOWS)
    FILETIME ft;
    HANDLE fh;
    fh = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, 
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(fh==INVALID_HANDLE_VALUE) {
        fe_loge(TAG, "Could not stat \"%s\".\n", path);
        return 0;
    }
    GetFileTime(fh, NULL, NULL, &ft);
    CloseHandle(fh);
    return (((uint64_t)ft.dwHighDateTime)<<32)+(uint64_t)ft.dwLowDateTime;
#else
    struct stat st;
    if(stat(path, &st)) {
        fe_logw(TAG, "Could not stat \"%s\".\n", path);
        return 0;
    }
    return st.st_mtime;
#endif
}

*/

fe_iov_status fe_fs_file_exists(const fe_iov_locator *params) {
#if defined(FE_TARGET_EMSCRIPTEN)
    bool exists = EM_ASM_INT({
        var path = Module.UTF8ToString($0);
        return FS.exists(path);
    }, params->file_name);
    return exists;
#else
    fe_iov_status status;
    status.step = FE_IOV_STEP_COMPLETED;
    fe_iov fullpath = {0};
    static_fullpath(&fullpath, params);
#if defined(FE_TARGET_WINDOWS)
    WCHAR *fullpath_w = fe_utf8_to_win32unicode(fullpath.base);
    if(!fullpath_w) {
        status.success = false;
        status.current = FE_IOV_FAILED_NO_MEM;
        return status;
    }
    status.success = (GetFileAttributesW(fullpath_w) != INVALID_FILE_ATTRIBUTES);
    fe_mem_heapfree(fullpath_w);
#else
    status.success = !access(fullpath.base, F_OK);
#endif
    status.current = status.success ? FE_IOV_SUCCESS : FE_IOV_FAILED_UNKNOWN;
    fe_mem_heapfree(fullpath.base);
    return status;
#endif
}

fe_iov_status  fe_fs_file_delete(const fe_iov_locator *params) {
#if defined(FE_TARGET_EMSCRIPTEN)
    EM_ASM_ARGS({
        var path = Module.UTF8ToString($0);
        FS.unlink(path);
    }, params->file_name);
#else
    fe_iov_status status;
    status.step = FE_IOV_STEP_COMPLETED;
    fe_iov fullpath = {0};
    static_fullpath(&fullpath, params);
#if defined(FE_TARGET_WINDOWS)
    WCHAR *fullpath_w = fe_utf8_to_win32unicode(fullpath.base);
    status.success = !!DeleteFileW(fullpath_w);
    fe_mem_heapfree(fullpath_w);
#else
    status.success = !unlink(fullpath.base);
#endif
    status.current = status.success ? FE_IOV_SUCCESS : FE_IOV_FAILED_UNKNOWN;
    fe_mem_heapfree(fullpath.base);
    return status;
#endif
}

fe_iov_status  fe_fs_persistent_exists(const fe_iov_locator *params) {
#ifdef FE_TARGET_EMSCRIPTEN
    int exists, error;
    emscripten_idb_exists(params->idb_name, params->file_name, &exists, &error);
    fe_iov_status status = {0}; 
    status.step = FE_IOV_STEP_COMPLETED;
    status.exists = exists;
    status.success = !error;
    status.current = error ? FE_IOV_FAILED_UNKNOWN : FE_IOV_SUCCESS;
    return status;
#else
    return fe_fs_file_exists(params);
#endif
}
fe_iov_promise fe_fs_persistent_exists_async(const fe_iov_locator *params) { return NULL; }
fe_iov_status  fe_fs_persistent_delete(const fe_iov_locator *params) {
#ifdef FE_TARGET_EMSCRIPTEN
    int error;
    emscripten_idb_delete(params->idb_name, params->file_name, &error);
    fe_iov_status status = {0}; 
    status.step = FE_IOV_STEP_COMPLETED;
    status.success = !error;
    status.current = error ? FE_IOV_FAILED_UNKNOWN : FE_IOV_SUCCESS;
    return status;
#else
    return fe_fs_file_delete(params);
#endif
}
fe_iov_promise fe_fs_persistent_delete_async(const fe_iov_locator *params) { return NULL; }


