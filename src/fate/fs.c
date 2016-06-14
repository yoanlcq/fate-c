#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <fate/defs.h>
#include <fate/log.h>
#include <fate/promise.h>
#include <fate/fs.h>

#if defined(FE_TARGET_WINDOWS)
#include <Windows.h>
#include <DbgHelp.h>
#elif defined(FE_TARGET_LINUX)
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>
#elif defined(FE_TARGET_FREEBSD)
#include <limits.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>
#elif defined(FE_TARGET_OSX)
#include <stdint.h>
#include <limits.h>
#include <signal.h>
#include <mach-o/dyld.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <execinfo.h>
#endif

static const char *TAG = "fe_fs";

fe_promise fe_userdata_exists(const char *filename) {return 0;}
fe_promise fe_userdata_delete(const char *filename) {return 0;}


/*
 *
 *
 *
 *
 *
 *
 *
 *    fe_fs_setcwd()
 *
 *
 *
 *
 *
 *
 *
 */


#if defined(FE_TARGET_EMSCRIPTEN)
inline bool fe_fs_setcwd(const char *path) {
    return false; /* TODO */
}
#elif defined(FE_TARGET_WINDOWS)
#if !(_MSC_VER && !__INTEL_COMPILER)
inline 
#endif 
bool fe_fs_setcwd(const char *path) {
    return SetCurrentDirectory(path);
}
#else
inline bool fe_fs_setcwd(const char *path) {
    return !chdir(path);
}
#endif

/*
 *
 *
 *
 *
 *
 *
 *
 *    fe_fs_exists()
 *
 *
 *
 *
 *
 *
 *
 */

#if defined(FE_TARGET_EMSCRIPTEN)
inline bool fe_fs_exists(const char *path) {
    FILE *file = fopen(path, "r");
    if(!file)
        return false;
    fclose(file);
    return true;
}
#elif defined(FE_TARGET_WINDOWS)
inline bool fe_fs_exists(const char *path) {
    return GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES;
}
#else
inline bool fe_fs_exists(const char *path) {
    return !access(path, F_OK);
}
#endif


/*
 *
 *
 *
 *
 *
 *
 *
 *    fe_fs_get_last_write_time()
 *
 *
 *
 *
 *
 *
 *
 */


#if defined(FE_TARGET_EMSCRIPTEN)
uint64_t fe_fs_get_last_write_time(const char *path) {
    return 0; /* FIXME */
}
#elif defined(FE_TARGET_WINDOWS)
uint64_t fe_fs_get_last_write_time(const char *path) {
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
}
#else
inline uint64_t fe_fs_get_last_write_time(const char *path) {
    struct stat st;
    if(stat(path, &st)) {
        fe_logw(TAG, "Could not stat \"%s\".\n", path);
        return 0;
    }
    return st.st_mtime;
}
#endif


void fe_fs_move(const char *dst, const char *src) {}
void fe_fs_copy(const char *dst, const char *src) {}
fe_filewatch fe_fs_watch(const char *filename) {return 0;}
void fe_fs_unwatch(fe_filewatch watch) {}
const char* fe_fs_getcwd() {return NULL;}
void fe_fs_pushd(const char *wd) {}
void fe_fs_popd(void) {}


