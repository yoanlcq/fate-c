#include <fate/iov.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#if defined(FE_TARGET_LINUX) && !defined(FE_TARGET_ANDROID)
#define NUL_FILE_PATH "/dev/null"
#elif defined(FE_TARGET_WINDOWS)
#define NUL_FILE_PATH "nul"
#endif
static FILE *nul_file = NULL;
void fe_iov_setup(void) {
#ifdef NUL_FILE_PATH
    nul_file = fopen(NUL_FILE_PATH, "wb");
#else
    nul_file = tmpfile();
#endif
    assert(nul_file);
}
void fe_iov_cleanup(void) {
    fclose(nul_file);
}

size_t  fe_iov_get_printf_len(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int cnt = vfprintf(nul_file, fmt, ap);
    assert(cnt >= 0);
    va_end(ap);
    fseek(nul_file, 0, SEEK_SET);
    return cnt;
}
void    fe_iov_deinit(fe_iov *iov) {}
void    fe_iov_resize(fe_iov *iov, size_t len) {}
int     fe_iov_printf(fe_iov *iov, size_t offset, const char *fmt, ...) {return 0;}
void    fe_iov_copy(fe_iov *iov, size_t offset, const fe_iov *src) {}
fe_iov_status  fe_iov_load_wget(fe_iov *iov, const char *url) {return 0;}
fe_iov_promise fe_iov_load_wget_async(fe_iov *iov, const char *url) {return 0;}
fe_iov_status  fe_iov_load_file(fe_iov *iov, const char *filename, fe_iov_rootdir rd) {return 0;}
fe_iov_promise fe_iov_load_file_async(fe_iov *iov, const char *filename, fe_iov_rootdir rd) {return 0;}
fe_iov_status  fe_iov_load_persistent(fe_iov *iov, const char *idb_name, const char *filename, fe_iov_rootdir rd) {return 0;}
fe_iov_promise fe_iov_load_persistent_async(fe_iov *iov, const char *idb_name, const char *filename, fe_iov_rootdir rd) {return 0;}
fe_iov_status  fe_iov_load_res(fe_iov *iov, const char *url, const char *filename, fe_iov_rootdir rd) {return 0;}
fe_iov_promise fe_iov_load_res_async(fe_iov *iov, const char *url, const char *filename, fe_iov_rootdir rd) {return 0;}
fe_iov_status  fe_iov_store_file(fe_iov *iov, const char *filename, fe_iov_rootdir rd) {return 0;}
fe_iov_promise fe_iov_store_file_async(fe_iov *iov, const char *filename, fe_iov_rootdir rd) {return 0;}
fe_iov_status  fe_iov_store_persistent(fe_iov *iov, const char *idb_name, const char *filename, fe_iov_rootdir rd) {return 0;}
fe_iov_promise fe_iov_store_persistent_async(fe_iov *iov, const char *idb_name, const char *filename, fe_iov_rootdir rd) {return 0;}


fe_iov_status  fe_persistent_exists(const char *idb_name, 
                                    const char *filename, 
                                    fe_iov_rootdir rd)
{
#ifdef FE_TARGET_EMSCRIPTEN
    int exists, error;
    emscripten_idb_exists(idb_name, filename, &exists, &error);
    return FE_IOV_STATUS_OK;
#endif
}

uintptr_t static_persistent_counter = 0;
static void static_exists_oncheck(void *arg, int exists) {}
static void static_exists_onerror(void *arg) {}

fe_iov_promise fe_persistent_exists_async(const char *idb_name, 
                                          const char *filename,
                                          fe_iov_rootdir rd)
{
#ifdef FE_TARGET_EMSCRIPTEN
    /* XXX We're not locking the counter because the browser environment
     * is not multi-threaded. */

    /*
    emscripten_idb_async_exists(idb_name, filename, 
        static_persistent_counter, 
        static_exists_oncheck, 
        static_exists_onerror        
    );
    ++static_persistent_counter;
    */
#else
    /* TODO */
#endif
}
fe_iov_status  fe_persistent_delete(const char *idb_name,
                                    const char *filename,
                                    fe_iov_rootdir rd)
{
#ifdef FE_TARGET_EMSCRIPTEN
    /*
    int error;
    emscripten_idb_delete(idb_name, filename, &error);
    return error ? FE_IOV_ERR_FAILED : FE_IOV_ERR_OK;
    */
#else
    /* TODO */
#endif
}

fe_iov_promise fe_persistent_delete_async(const char *idb_name, const char *filename, fe_iov_rootdir rd) {return 0;}


fe_fd          fe_fd_open_file(const char *filename, fe_iov_rootdir rd, fe_fd_flags flags) {return 0;}
fe_fd          fe_fd_open_persistent(const char *idb_name, const char *filename, fe_iov_rootdir rd, fe_fd_flags flags) {return 0;}
fe_fd          fe_fd_open_res(const char *url, const char *filename, fe_iov_rootdir rd, fe_fd_flags flags) {return 0;}
fe_iov_promise fe_fd_get_download_promise(fe_fd fd) {return 0;}
bool           fe_fd_is_valid(fe_fd fd) {return 0;}
void*          fe_fd_mmap(fe_fd fd, fe_fd_offset offset, size_t len, bool rw) {return 0;}
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
void fe_iov_promise_notify(fe_iov_promise p, fe_iov_promise_completion_routine callback, void *userdata) {}
void fe_iov_promise_cancel(fe_iov_promise p) {}

