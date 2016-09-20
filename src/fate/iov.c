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

/* Some of the included headers are probably useless for
 * this file, but I'm too lazy to find which. */

#if defined(FE_TARGET_WINDOWS)
#include <Windows.h>
#elif defined(FE_TARGET_LINUX)
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#elif defined(FE_TARGET_FREEBSD)
#include <limits.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#elif defined(FE_TARGET_OSX) || defined(FE_TARGET_IOS)
#include <stdint.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <mach-o/dyld.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <errno.h>
#endif



#define TAG "fe_iov"


#if defined(FE_TARGET_LINUX) && !defined(FE_TARGET_ANDROID)
#define NUL_FILE_PATH "/dev/null"
#elif defined(FE_TARGET_WINDOWS)
#define NUL_FILE_PATH "nul"
#endif
#define NUL_FILE_FALLBACK_NAME ".nul"
static FILE *nul_file = NULL;
static char *nul_file_path = "";

static char *static_sdl_basepath = NULL;

void fe_iov_setup(void) {
    static_sdl_basepath = SDL_GetBasePath();
    if(!static_sdl_basepath)
        static_sdl_basepath = SDL_strdup("./");
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
    /* BTW, while we're at it... */
    mkdir(SDL_AndroidGetExternalStoragePath(), 0755);
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
    fe_logi(TAG, "Opening `%s' as the null file "
                 "for fe_iov_get_vprintf_len().\n", nul_file_path);
    nul_file = fopen(nul_file_path, "wb");
    fe_dbg_hope(nul_file);
}
void fe_iov_cleanup(void) {
    SDL_free(static_sdl_basepath);
    fclose(nul_file);
#ifndef NUL_FILE_PATH
    #ifndef FE_TARGET_EMSCRIPTEN
        unlink(nul_file_path);
    #endif
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

    if(offset+len > iov->len)
        fe_iov_resize(iov, offset+len);
    
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


#ifdef FE_DEBUG_BUILD

typedef struct {
    bool correct : 1;
    bool android_bad : 1;
    bool winrt_bad : 1;
} static_rootdir_validation;

static static_rootdir_validation static_rootdir_validate(fe_iov_rootdir rootdir) {
    static_rootdir_validation v = {0};
    if((rootdir & FE_IOV_ROOTDIR_SDL2_GETPREFPATH)
    || (rootdir & FE_IOV_ROOTDIR_SDL2_GETBASEPATH)) {
        v.correct = true;
        return v;
    }
    bool android_good = (rootdir & FE_IOV_ROOTDIR_ANDROID_INTERNAL_STORAGE)
                     || (rootdir & FE_IOV_ROOTDIR_ANDROID_EXTERNAL_STORAGE);
    if(!android_good)
        v.android_bad = true;
    /* XXX What about WinRT ? */
    return v;
}
#endif

static void static_fullpath(fe_iov *fullpath, const fe_iov_locator *params) {
#ifdef FE_DEBUG_BUILD
    static_rootdir_validation v = static_rootdir_validate(params->rootdir);
    if(!v.correct) {
        fe_fatal(TAG, "Did not attempt to open `%s', because "
                "rootdir flags are incomplete :\n%s%s",
                params->file_name,
                v.android_bad ? "On Android, specifying whether to use "
                "internal or external storage is required, and you "
                "didn't use the flags for "
                "SDL2's GetBasePath() and GetPrefPath() either.\n" : "",
                v.winrt_bad ? "" : "");
    }
#endif
#define HELPER(cst, call, free_maybe) \
    if(params->rootdir & cst) { \
        char *ppath = call;\
        fe_iov_printf(fullpath, 0, "%s/%s", ppath, params->file_name); \
        free_maybe(ppath);\
        return; \
    }
    HELPER(FE_IOV_ROOTDIR_SDL2_GETPREFPATH, SDL_GetPrefPath(params->organization, params->app_name), SDL_free)
    HELPER(FE_IOV_ROOTDIR_SDL2_GETBASEPATH, static_sdl_basepath, (void))
#ifdef FE_TARGET_ANDROID
    HELPER(FE_IOV_ROOTDIR_ANDROID_INTERNAL_STORAGE, SDL_AndroidGetInternalStoragePath(), (void))
    HELPER(FE_IOV_ROOTDIR_ANDROID_EXTERNAL_STORAGE, SDL_AndroidGetExternalStoragePath(), (void))
#elif defined(FE_TARGET_WINDOWS_RT)
    HELPER(FE_IOV_ROOTDIR_WINRT_PATH_LOCAL_FOLDER, SDL_WinRTGetFSPathUTF8(SDL_WINRT_PATH_LOCAL_FOLDER), (void))
    HELPER(FE_IOV_ROOTDIR_WINRT_PATH_INSTALLED_LOCATION, SDL_WinRTGetFSPathUTF8(SDL_WINRT_PATH_INSTALLED_LOCATION), (void))
#endif
#undef HELPER
}


#ifdef FE_TARGET_ANDROID
static const char* static_fd_flags_to_stdio_mode(fe_fd_flags flags) {
    if(flags & FE_FD_OPEN_APPEND) {
        if(flags & FE_FD_OPEN_READONLY)
            return "a+b";
        return "ab";
    }
    if(flags & FE_FD_OPEN_TRUNCATE) {
        if(flags & FE_FD_OPEN_READONLY)
            return "w+b";
        return "wb";
    }
    if(flags & FE_FD_OPEN_READWRITE)
        return "r+b"; /* XXX This can't create a file, but it's not 
                       *  a problem for now because our use of this
                       *  function is limited to Android assets which 
                       *  are read-only. */
    return "r";
}
#endif /* FE_TARGET_ANDROID */



fe_iov_status  fe_iov_load_wget(fe_iov *iov, const fe_iov_locator *params) {
#ifdef FE_TARGET_EMSCRIPTEN
    int len, error;
    void *ptr = iov->base; /* Warning for incompatible ptr types. */
    emscripten_wget_data(params->file_url, &ptr, &len, &error);
    iov->len = len;
    fe_iov_status status = {0};
    status.step = FE_IOV_STEP_COMPLETED;
    status.success = !error;
    status.current = error ? FE_IOV_FAILED_UNKNOWN : FE_IOV_SUCCESS;
    return status;
#else
    /* TODO */
    fe_dbg_hope(0 && "This has not yet been implemented!!");
    fe_iov_status status = {0}; return status;
#endif
}

fe_iov_promise fe_iov_load_wget_async(fe_iov *iov, const fe_iov_locator *params) {return NULL;}


static fe_iov_status static_iov_load(fe_iov *iov, const fe_iov_locator *params, fe_fd (*do_open)(const fe_iov_locator*)) {
    fe_iov_status status = {0};
    status.step = FE_IOV_STEP_COMPLETED;
    fe_iov_locator params_real = *params;
    params_real.fd_flags |= FE_FD_OPEN_READONLY;
    params_real.fd_flags |= FE_FD_OPEN_SEQUENTIAL_ACCESS_HINT;
    params_real.fd_flags |= FE_FD_OPEN_SINGLE_ACCESS_HINT;
    params_real.fd_flags |= FE_FD_OPEN_NEAR_FUTURE_ACCESS_HINT;
    params_real.fd_flags |= FE_FD_OPEN_EAGER_DOWNLOAD;
    fe_fd fd = do_open(&params_real);
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

fe_iov_status  fe_iov_load_file(fe_iov *iov, const fe_iov_locator *params) {
    return static_iov_load(iov, params, fe_fd_open_file);
}

fe_iov_promise fe_iov_load_file_async(fe_iov *iov, const fe_iov_locator *params) {return NULL;}



fe_iov_status  fe_iov_load_persistent(fe_iov *iov, const fe_iov_locator *params) {
#ifdef FE_TARGET_EMSCRIPTEN
    int len, error;
    void *ptr = iov->base; /* For incompatible ptr types warning. */
    emscripten_idb_load(params->idb_name, params->file_name, &ptr, &len, &error);
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
#elif defined(FE_TARGET_ANDROID)
    return static_iov_load(iov, params, fe_fd_open_res);
#else
    return fe_iov_load_file(iov, params);
#endif
}

fe_iov_promise fe_iov_load_res_async(fe_iov *iov, const fe_iov_locator *params) {return NULL;}



fe_iov_status  fe_iov_store_file(fe_iov *iov, const fe_iov_locator *params) {
    fe_iov_status status = {0};
    status.step = FE_IOV_STEP_COMPLETED;
    fe_iov_locator params_real = *params;
    params_real.fd_flags |= FE_FD_OPEN_WRITEONLY;
    params_real.fd_flags |= FE_FD_OPEN_SEQUENTIAL_ACCESS_HINT;
    params_real.fd_flags |= FE_FD_OPEN_SINGLE_ACCESS_HINT;
    params_real.fd_flags |= FE_FD_OPEN_NEAR_FUTURE_ACCESS_HINT;
    fe_fd fd = fe_fd_open_file(&params_real);
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


#ifdef FE_TARGET_WINDOWS
static fe_fd static_fe_fd_open_file_windows(const char *fullpath, fe_fd_flags fd_flags) {
    DWORD desired_access = 0;
    desired_access |= GENERIC_READ  * !!(fd_flags & FE_FD_OPEN_READONLY);
    desired_access |= GENERIC_WRITE * !!(fd_flags & FE_FD_OPEN_WRITEONLY);
    DWORD share = 0;
    share |= FILE_ALLOW_DELETE * !!(fd_flags & FE_FD_OPEN_W32_SHARE_DELETE);
    share |= FILE_ALLOW_READ   * !!(fd_flags & FE_FD_OPEN_W32_SHARE_READ);
    share |= FILE_ALLOW_WRITE  * !!(fd_flags & FE_FD_OPEN_W32_SHARE_WRITE);
    DWORD creation_disposition = 0;
    creation_disposition |= OPEN_ALWAYS * !!(fd_flags & FE_FD_OPEN_WRITEONLY);
    creation_disposition |= CREATE_NEW  * !!(fd_flags & FE_FD_OPEN_ENSURE_NONEXISTENT);
    DWORD attrs = FILE_ATTRIBUTE_NORMAL;
    DWORD flags = 0;
    WCHAR *fullpath_w = fe_utf8_to_win32unicode(fullpath);
    fe_dbg_hope(fullpath_w);
    fe_fd fd = CreateFileW(fullpath_w, desired_access, share, 
            NULL, creation_disposition, flags | attrs, NULL);
    fe_mem_heapfree(fullpath_w);
    return fd;
}
#elif defined(FE_TARGET_EMSCRIPTEN)
static fe_fd static_fe_fd_open_file_stdio(const char *fullpath, fe_fd_flags fd_flags) {
    fe_dbg_hope(0 == "This is not implemented yet!");
}
#else
static fe_fd static_fe_fd_open_file_unix(const char *fullpath, fe_fd_flags fd_flags) {
    int flags = 0;
    switch(fd_flags & FE_FD_OPEN_READWRITE) {
    case FE_FD_OPEN_READONLY:  flags |= O_RDONLY; break;
    case FE_FD_OPEN_WRITEONLY: flags |= O_WRONLY | O_CREAT; break;
    case FE_FD_OPEN_READWRITE: flags |= O_RDWR | O_CREAT; break;
    }
    flags |= O_APPEND*!!(fd_flags & FE_FD_OPEN_APPEND);
    flags |= O_EXCL  *!!(fd_flags & FE_FD_OPEN_ENSURE_NONEXISTENT);
    flags |= O_TRUNC *!!(fd_flags & FE_FD_OPEN_TRUNCATE);

    fe_fd fd = open(fullpath, flags, 0666);
    #if _XOPEN_SOURCE >= 600 || _POSIX_C_SOURCE >= 200112L
        if(fd_flags & FE_FD_OPEN_RANDOM_ACCESS_HINT)
            posix_fadvise(fd, 0, 0, POSIX_FADV_RANDOM);
        if(fd_flags & FE_FD_OPEN_SEQUENTIAL_ACCESS_HINT)
            posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
        if(fd_flags & FE_FD_OPEN_SINGLE_ACCESS_HINT)
            posix_fadvise(fd, 0, 0, POSIX_FADV_NOREUSE);
        if(fd_flags & FE_FD_OPEN_NEAR_FUTURE_ACCESS_HINT)
            posix_fadvise(fd, 0, 0, POSIX_FADV_WILLNEED);
        if(fd_flags & FE_FD_OPEN_FAR_FUTURE_ACCESS_HINT)
            posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED);
    #endif
    return fd;
}
#endif /* FE_TARGET_WINDOWS */

#ifdef FE_TARGET_ANDROID

static void static_sdlrwops_setcallbacks(SDL_rwops *rw);

static fe_fd static_fe_fd_open_file_android(const char *fullpath, fe_fd_flags fd_flags) {
    int unix_fd = static_fe_fd_open_file_unix(fullpath, fd_flags);
    if(unix_fd < 0)
        return FE_FD_INVALID_FD;
    fe_fd fd = SDL_AllocRW();
    fd->type  = SDL_RWOPS_UNKNOWN;
    fd->hidden.unknown.data1 = (void*)(uintptr_t) unix_fd;
    #define static_sdlrwops_has_unix_fd(fd) ((fd)->type == SDL_RWOPS_UNKNOWN)
    #define static_sdlrwops_get_unix_fd(fd) ((unsigned)(uintptr_t)(fd)->hidden.unkown.data1)
    fd->hidden.unknown.data2 = NULL;
    static_sdlrwops_setcallbacks(fd);
}

static Sint64 SDLCALL static_sdlrwops_size(struct SDL_RWops * context) {
    Sint64 cur = lseek(static_sdlrwops_get_unix_fd(context), 0, SEEK_CUR);
    Sint64 end = lseek(static_sdlrwops_get_unix_fd(context), 0, SEEK_END);
    lseek(static_sdlrwops_get_unix_fd(context), cur, SEEK_SET);
    return end;
}

static Sint64 SDLCALL static_sdlrwops_seek(struct SDL_RWops * context, Sint64 offset,
                             int whence) {
    return lseek(static_sdlrwops_get_unix_fd(context), offset, whence);
}

static size_t SDLCALL static_sdlrwops_read(struct SDL_RWops * context, void *ptr,
                             size_t size, size_t maxnum) {
    ssize_t bytes_read = read(static_sdlrwops_get_unix_fd(context), ptr, size*maxnum);
    return bytes_read<=0 ? 0 : bytes_read/size;
}

static size_t SDLCALL static_sdlrwops_write(struct SDL_RWops * context, const void *ptr,
                          size_t size, size_t num) {
    ssize_t bytes_written = write(static_sdlrwops_get_unix_fd(context), ptr, size*num);
    return bytes_written<=0 ? 0 : bytes_written/size;
}

static int SDLCALL static_sdlrwops_close(struct SDL_RWops * context) {
    int ret = close(static_sdlrwops_get_unix_fd(context));
    SDL_FreeRW(context);
    return ret;
}

static void static_sdlrwops_setcallbacks(SDL_rwops *fd) {
    fd->size  = static_sdlrwops_size;
    fd->seek  = static_sdlrwops_seek;
    fd->read  = static_sdlrwops_read;
    fd->write = static_sdlrwops_write;
    fd->close = static_sdlrwops_close;
}

#endif


fe_fd          fe_fd_open_file(const fe_iov_locator *params) {
    fe_fd fd = FE_FD_INVALID_FD;
    fe_iov fullpath;
    static_fullpath(&fullpath, params);
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    fd = static_fe_fd_open_file_windows(fullpath.base, params->fd_flags);
#elif defined(FE_TARGET_ANDROID)
    fd = static_fe_fd_open_file_android(fullpath.base, params->fd_flags);
#else
    fd = static_fe_fd_open_file_unix(fullpath.base, params->fd_flags);
#endif
    fe_mem_heapfree(fullpath.base);
    return fd;
}

fe_fd          fe_fd_open_persistent(const fe_iov_locator *params) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#else
    return fe_fd_open_file(params);
#endif
}

fe_fd          fe_fd_open_res(const fe_iov_locator *params) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_ANDROID)
    /* XXX
     * SDL2 checks internal storage first, and only falls back to assets/
     * if the file is not in internal storage. 
     * Writes are denied in the assets/ folder, unlike internal storage.
     * Also, files in assets/ are closer to being 'resources' than
     * files in internal storage.
     * The only reasonable thing to do is assume that the API's user
     * distinguishes resources and regular files by storing them in
     * distinct directories. Something like :
     * "All resources start with res/"
     * "All regular files start with files/".
     * This behaviour needs to be documented.
     */
    return SDL_RWFromFile(
        params->file_name, 
        static_fd_flags_to_stdio_mode(params->fd_flags)
    );
#else
    return fe_fd_open_file(params);
#endif   
}

fe_iov_promise fe_fd_get_download_promise(fe_fd fd) {
    fe_dbg_hope(0 && "This is not implemented yet !");
    return NULL;
}

bool   fe_fd_mmap(fe_filemapview *v, fe_fd fd, fe_fd_offset offset, size_t len, bool rw) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    fe_dbg_assert(fe_fd_get_length(fd) > 0);
    /* XXX MSDN suggests that the file should be opened with exclusive access, to prevent
     * other processes from writing to it. */
    v->filemapping = CreateFileMapping(fd, NULL, 
            /* There are a lot of other nice-looking flags that we should inspect. */
            rw ? PAGE_READWRITE : PAGE_READONLY,
            sizeof(len)>4 ? len>>32 : 0, len,
            NULL /* Optional name for the system-wide file mapping object */
    );
    if(!v->filemapping)
        return false; /* Info in GetLastError(). */
    static DWORD dwSysGran = 0;
    if(!dwSysGran) {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        dwSysGran = si.dwAllocationGranularity;
    }
    uint64_t fileMapStart = (offset / dwSysGran) * dwSysGran;
    DWORD dwMapViewSize = (offset % dwSysGran) + len;
    void *iViewDelta = offset - dwFileMapStart;
    v->view_base = MapViewOfFile(fmap, 
            FILE_MAP_READ | (rw*FILE_MAP_WRITE),
            fileMapStart>>32, fileMapStart, dwMapViewSize
    );
    if(!v->view_base)
        return false;
    v->base = v->view_base + iViewDelta;
    v->len = len;
    return true;
#elif defined(FE_TARGET_ANDROID)
    if(!static_sdlrwops_has_unix_fd(fd))
        return false; /* Can't mmap() a compressed file. */
    v->base = mmap(NULL, len, PROT_READ | (rw*PROT_WRITE), MAP_PRIVATE, static_sdlrwops_get_unix_fd(fd), offset);
    v->len = len;
    return v->base != MAP_FAILED;
#else
    v->base = mmap(NULL, len, PROT_READ | (rw*PROT_WRITE), MAP_PRIVATE, fd, offset);
    v->len = len;
    return v->base != MAP_FAILED;
#endif
}
void fe_fd_munmap(fe_filemapview *v) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    UnmapViewOfFile(v->view_base);
    CloseHandle(v->filemapping);
#else
    munmap(v->base, v->len);
#endif
}

fe_fd_offset   fe_fd_seek(fe_fd fd, fe_fd_offset offset, fe_fd_seek_whence whence) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    LARGE_INTEGER req_offset, new_offset;
    req_offset.QuadPart = offset;
    BOOL success = SetFilePointerEx(fd, req_offset, &new_offset, whence);
    fe_dbg_hope(success); /* XXX */
    return new_offset.QuadPart;
#elif defined(FE_TARGET_ANDROID)
    return SDL_RWSeek(fd, offset, whence);
#else
    return lseek(fd, offset, whence);
#endif
}
ssize_t        fe_fd_read(fe_fd fd, void *buf, size_t len) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    DWORD bytes_read;
    BOOL success = ReadFile(fd, buf, len, &bytes_read, NULL);
    return success ? bytes_read : -1;
#elif defined(FE_TARGET_ANDROID)
    return SDL_RWread(fd, buf, 1, len);
#else
    return read(fd, buf, len);
#endif
}
ssize_t        fe_fd_write(fe_fd fd, const void *buf, size_t len) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    DWORD bytes_written;
    BOOL success = WriteFile(fd, buf, len, &bytes_written, NULL);
    return success ? bytes_read : -1;
#elif defined(FE_TARGET_ANDROID)
    return SDL_RWwrite(fd, buf, 1, len);
#else
    return write(fd, buf, len);
#endif
}
ssize_t        fe_fd_readv(fe_fd fd, fe_iov *iov_array, size_t iov_count) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS) || defined(FE_TARGET_ANDROID)
    /* ReadFileScatter() doesn't do what we want. It can only fill 
     * page-sized buffers, and it is asynchronous. */
    ssize_t bytes_read;
    size_t i;
    for(bytes_read=i=0 ; i<iov_count ; ++i)
        bytes_read += fe_fd_read(fd, iov_array[i].base, iov_array[i].len);
    return bytes_read;
#else
    return readv(fd, (struct iovec*)iov_array, iov_count);
#endif
}
ssize_t        fe_fd_writev(fe_fd fd, const fe_iov *iov_array, size_t iov_count) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS) || defined(FE_TARGET_ANDROID)
    ssize_t bytes_written;
    size_t i;
    for(bytes_written=i=0 ; i<iov_count ; ++i)
        bytes_written += fe_fd_write(fd, iov_array[i].base, iov_array[i].len);
    return bytes_written;
#else
    return writev(fd, (struct iovec*)iov_array, iov_count);
#endif
}
ssize_t        fe_fd_preadv(fe_fd fd, fe_iov *iov_array, size_t iov_count, fe_fd_offset offset) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS) || defined(FE_TARGET_ANDROID)
    fe_fd_offset old = fe_fd_seek(fd, 0, FE_FD_SEEK_CUR);
    fe_fd_seek(fd, offset, FE_FD_SEEK_SET);
    ssize_t bytes_read = fe_fd_readv(fd, iov_array, iov_count);
    fe_fd_seek(fd, old, FE_FD_SEEK_SET);
    return bytes_read;
#else
    return preadv(fd, (struct iovec*)iov_array, iov_count, offset);
#endif
}
ssize_t        fe_fd_pwritev(fe_fd fd, const fe_iov *iov_array, size_t iov_count, fe_fd_offset offset) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS) || defined(FE_TARGET_ANDROID)
    fe_fd_offset old = fe_fd_seek(fd, 0, FE_FD_SEEK_CUR);
    fe_fd_seek(fd, offset, FE_FD_SEEK_SET);
    ssize_t bytes_written = fe_fd_writev(fd, iov_array, iov_count);
    fe_fd_seek(fd, old, FE_FD_SEEK_SET);
    return bytes_written;
#else
    return pwritev(fd, (struct iovec*)iov_array, iov_count, offset);
#endif
}
bool           fe_fd_sync(fe_fd fd) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    return FlushFileBuffers(fd);
#elif defined(FE_TARGET_ANDROID)
    if(!static_sdlrwops_has_unix_fd(fd))
        return false;
    return !fsync(static_sdlrwops_get_unix_fd(fd));
#else
    return !fsync(fd);
#endif
}
bool           fe_fd_truncate(fe_fd fd, size_t len) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    fe_fd_offset cur = fe_fd_seek(fd, 0, FE_FD_SEEK_CUR);
    fe_fd_seek(fd, len, FE_FD_SEEK_SET);
    SetEndOfFile(fd);
    fe_fd_seek(fd, 0, cur);
#elif defined(FE_TARGET_ANDROID)
    if(!static_sdlrwops_has_unix_fd(fd))
        return false;
    return !ftruncate(static_sdlrwops_get_unix_fd(fd), len);
#else
    return !ftruncate(fd, len);
#endif
}
void           fe_fd_close(fe_fd fd) {
#ifdef FE_TARGET_EMSCRIPTEN
    /* XXX Should Log a warning if fe_fd_sync() was not called. */
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    CloseHandle(fd);
#elif defined(FE_TARGET_ANDROID)
    SDL_RWclose(fd);
#else
    if(!close(fd))
        return;
    switch(errno) {
    case EINTR: fe_loge(TAG, "close() failed with %s !\n", "EINTR"); break;
    case EIO:   fe_loge(TAG, "close() failed with %s !\n", "EIO");   break;
    }
#endif
}

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
    fe_iov fullpath = FE_IOV_ZERO_INITIALIZER;
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
    fe_iov_status status = {0};
    status.exists = EM_ASM_INT({
        var path = Module.UTF8ToString($0);
        return FS.exists(path);
    }, params->file_name);
    return status;
#else
    fe_iov_status status;
    status.step = FE_IOV_STEP_COMPLETED;
    fe_iov fullpath = FE_IOV_ZERO_INITIALIZER;
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
    fe_iov_status status = {0};
    status.step = FE_IOV_STEP_COMPLETED;
#if defined(FE_TARGET_EMSCRIPTEN)
    EM_ASM_ARGS({
        var path = Module.UTF8ToString($0);
        FS.unlink(path);
    }, params->file_name);
#else
    fe_iov fullpath = FE_IOV_ZERO_INITIALIZER;
    static_fullpath(&fullpath, params);
    #if defined(FE_TARGET_WINDOWS)
        WCHAR *fullpath_w = fe_utf8_to_win32unicode(fullpath.base);
        status.success = !!DeleteFileW(fullpath_w);
        fe_mem_heapfree(fullpath_w);
    #else
        status.success = !unlink(fullpath.base);
    #endif
    fe_mem_heapfree(fullpath.base);
#endif
    status.current = status.success ? FE_IOV_SUCCESS : FE_IOV_FAILED_UNKNOWN;
    return status;
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


