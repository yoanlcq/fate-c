/* 
 * This file is part of F.A.T.E - the Fight And Travel game Engine.
 * Copyright (c) 2015-2016 Yoan Lecoq (yoanlecoq.io at gmail.com)
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented;
 *    you must not claim that you wrote the original software. 
 *    If you use this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, 
 *    and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * Contributors :
 * - Please make sure your indentation is set to 4 spaces;
 * - Please avoid exceeding 80 character columns.
 *
 */

/*! \file fate/iov.h
 *  \brief The I/O module.
 *  \defgroup iov Synchronous I/O, Asynchronous I/O and I/O Vectors (memory streams).
 *
 * This module offers four major APIs :
 * - The generic #fe_iov struct, representing a buffer and a length, alongside with 
 *   utilities to make it behave like a dynamic null-terminated memory stream.
 *   "iov" stands for I/O Vector and reminds of the \c struct \c iovec used by
 *   the \c readv() system call;
 * - The fe_iov_load* and fe_iov_store* set of functions, for reading whole files into
 *   memory at once, as well as overwriting completely such files; This maps very well
 *   to Emscripten's API while being really useful for native programs.
 * - The fe_fd* (fd standing for the Unix-y File Descriptor) set of functions, abstracting 
 *   OS-specific handles to files, and providing more complete functionality than the 
 *   standard IO library. However, there are some quirks on Emscripten 
 *   (which are no big deal but might just look annoying).
 * - #fe_iov_promise objects and their API, representing asynchronous I/O operations
 *   in progress.
 *
 * TODO clean up (and update ???) this documentation.
 *
 * The fe_iov_load* and fe_iov_store* APIs represent the common denominator across platforms for I/O.
 * That is, they are asynchronous, and operate on no less than whole files at once.
 * You're not always expected to always use them as-is though. You'll likely want consistent
 * behaviour and define your game-specific wrappers :
 * \code{.c}
 * #define cj_iov_load_res(iov, filename) fe_iov_load_res(iov, CJ_GAMEINFO_SITE_URL, filename, FE_IOV_SDL2_GETBASEPATH)
 * #define cj_iov_load_persistent(iov, filename) fe_iov_load_res(iov, CJ_GAMEINFO_IDB_NAME, filename, FE_IOV_SDL2_GETPREFPATH)
 * // Etc.
 * \endcode
 *
 *
 * Keep in mind you can use a \c fe_iov like the simple buffer+length combination it actually is.
 * You could just do things like :
 *
 * \code{.c}
 * fe_iov iov = { "Hello!", 6 }; //Don't care about '\0', we're writing to a file.
 * fe_iov_store_persistent(&iov, ...);
 * \endcode
 *
 * Or even utilities such as :
 * \code{.c}
 * static inline fe_iov_promise cj_store_persistent_string(const char *filename, const char *str) {
 *     fe_iov iov = { str, strlen(str) };
 *     return fe_iov_store_persistent(&iov, CJ_GAMEINFO_IDB_NAME, filename, FE_IOFF_SDL2_GETPREFPATH);
 * }
 * \endcode
 *
 * Want to preserve filenames across calls ? Write your own \c fe_iov wrapper :
 * \code{.c}
 * typedef struct {
 *     fe_iov iov;
 *     const char *filename;
 *     // etc...
 * }; cj_iov;
 * \endcode
 *
 * The suffixes map to the following interfaces :
 * - wget: Downloads a file from the web into a memory stream, no matter the platform;
 * - file: Opens a file just like with fopen(). This maps to MEMFS on Emscripten and requires special care;
 * - persistent: IndexedDB on Emscripten, a file otherwise (see above);
 * - res: Wget on Emscripten (see above), a file otherwise (see above).
 *
 *
 *
 * The fe_fd API's goal is to achieve efficient I/O on targets other than Emscripten (while still working on Emscripten, though).
 *
 * Differences with the iov_load/iov_store APIs :
 * -> This wrapper is closer to a native runtime, in that it maps nearly 1-to-1 to system calls
 *    for all targets other than Emscripten.
 * -> A fe_fd is lightweight, and always refers to a same resource (if you want to edit a file in-place, you don't have to
 *    keep track of the file's name).
 *    Here's what a fe_fd actually is, depending on the target :
 *    - Emscripten : Pointer to heap-allocated struct { union {url,idb_name}, filename }.
 *    - *nix : an int (file descriptor).
 *    - Windows : a HANDLE (file handle).
 * -> You have to use readv() and stuff because the file's content is supposedly managed by the OS, or not yet in memory.
 *    However, on Emscripten, you can't download less than full files. That means the whole file's content has to be put
 *    into memory, even if you only need a specific part.
 *    Since readv() copies data into buffers you own, that's an unavoidable waste of memory (also quite like
 *    operating systems may manage their own file contents cache).
 * -> The same applies to writev(), by the way.
 * -> On Emscripten, fe_fd_mmap() just returns a pointer to downloaded memory, avoiding the overhead of copying data.
 *    However, in other environments, this should be slower than a single read() since the OS has to manage an actual memory map.
 * -> Files are not guaranteed to be overwritten unless you call fe_fd_sync(). fe_fd_munmap() does not do implicitly do it, nor does fe_fd_close().
 * -> You can provide more hints about your access patterns.
 *
 * Here's a good example usage :
 * \code{.c}
 * static const char *TAG = "fe_fd example";
 * fe_fd fd = fe_fd_open_file("foo.txt", 0, FE_FD_OPEN_WRITEONLY | FE_FD_OPEN_SEQUENTIAL_ACCESS_HINT);
 * if(!fe_fd_is_valid(fd))
 *     fe_fatal(TAG, "Could not open foo.txt !!!\n");
 * fe_fd_truncate(fd, 6); //Just for fun. Not actually needed here.
 * fe_fd_write(fd, "Hello!", 6);
 * fe_iov_promise_state st;
 * fe_iov_promise_wait(fe_fd_sync(fd), &st, -1);
 * // Actual code should check for st.status here.
 * fe_fd_close(fd);
 * \endcode
 *
 * In very serious situations (e.g having to read huge files), you should use ifdefs
 * to choose the best way to operate depending on the target platform.
 * For instance, it's probably a waste to call #fe_fd_truncate() for a persistent file
 * on Emscripten.
 * #fe_fd_mmap() is the best way to go on Emscripten, but probably not in a native runtime.
 *
 * There's no fe_fd_open_wget() call because you can't just open a file descriptor to a Wget
 * download within a native runtime.
 *
 * #fe_fd_open_res() will synchronously return an identifier to a Wget download on Emscripten.
 * Unless the FE_FD_OPEN_LAZY_DOWNLOAD was set, a download promise for it is enqueued.
 * Get the promise with fe_fd_get_download_promise(fe_fd fd) and wait for it to complete.
 * If you don't feel like bothering with it, you can use the FE_FD_OPEN_EAGER_DOWNLOAD flag, which
 * will cause the open() operation to synchronously download the file.
 *
 * So here's an example for opening a res file :
 * \code{.c}
 * fe_fd fd = fe_fd_open_res("http://www.foo.org/", "foo.txt", 0, FE_FD_OPEN_READONLY);
 * #ifdef FE_TARGET_EMSCRIPTEN
 * { 
 *     fe_iov_promise_state st;
 *     fe_iov_promise_wait(fe_fd_get_download_promise(fd), &st, -1);
 *     // Check for st.status here !
 * }
 * #else
 * if(!fe_fd_is_valid(fd))
 *     ; // Fail miserably !
 * #endif
 * char buf1[16], buf2[32];
 * fe_iov iovs[] = {
 *     {buf1, sizeof(buf1)},
 *     {buf2, sizeof(buf2)}
 * }
 * ssize_t nread = fe_fd_readv(fd, iovs, 2);
 * // Make sure that nread is what you expect !
 * fe_fd_close(fd);
 * \endcode
 *
 *
 *
 *
 * Both #fe_iov_promise_poll and #fe_iov_promise_wait() return whether you should try again 
 * (false if the promise is complete, true otherwise),
 * such that you could achieve an active wait like so :
 * \code{.c}
 * while(fe_iov_promise_poll(p, &st));
 * // The promise is now freed, the I/O operations has completed.
 * \endcode
 *
 * Promises are freed by one of the following :
 * - #fe_iov_promise_poll() when it returns false;
 * - #fe_iov_promise_wait() when it returns false;
 * - #fe_iov_promise_cancel();
 *
 * Performing an operation on a freed promise does not result in undefined behaviour, but
 * it's an error. TODO more details needed.
 *
 * Waiting on a same promise from multiple threads should be possible, but since
 * I haven't tested, I strongly advise against it. If possible, the implementation
 * could use reference counting to allow this behaviour.
 *
 * Depending on the implementation, #fe_iov_promise_poll() may be just defined as #fe_iov_promise_wait() with
 * a zero timeout.
 *
 * #fe_iov_promise_wait():
 * - WaitForSingleObject() on the hEvent member of the OVERLAPPED parameter of ReadFileEx() and friends.
 * - select(), poll(), epoll()...
 *
 * #fe_iov_promise_notify() can be used to request that a callback be called from 
 * an indeterminate thread when the promise completes. It doesn't free the promise.
 * Calling #fe_iov_promise_notify() when the promise is complete results in the callback being
 * called immediately. In its own way, #fe_iov_promise_notify() is race-free.
 * - Win32: OVERLAPPED_COMPLETION_ROUTINE parameter of ReadFileEx() and friends.
 *
 * #fe_iov_promise_cancel() attempts to cancel the I/O operation associated with a promise.
 * The call returns immediately. Note that it cannot magically "undo" the operation. As you
 * should expect, the content of the buffers written to by the I/O operation must be considered 
 * undefined.
 * If the promise was already completed at the time of a call to #fe_iov_promise_cancel(), the
 * promise is simply freed.
 * - Win32: CancelIO();
 * - POSIX AIO: aio_cancel();
 * - Emscripten: emscripten_async_wget2_abort() and friends;
 *
 *
 * @{
 */

#ifndef FE_IOV_H
#define FE_IOV_H

#include <fate/fate.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

/*! \brief TODO*/
void fe_iov_setup(void);
/*! \brief TODO*/
void fe_iov_cleanup(void);

/*! \brief Get the number of bytes that a printf() call would output.
 *
 * This is used internally by this module, and may prove useful to the user as well.
 */
size_t fe_iov_get_printf_len(const char *fmt, ...);
size_t fe_iov_get_vprintf_len(const char *fmt, va_list ap);

/*! \brief A buffer+length combination (iov standing for I/O Vector) 
 *
 * This is a POD structure which members are guaranteed to stay the same.
 * Tu use it as a dynamic memory stream, simply initialize it to 0.
 * This works because #fe_iov_resize() uses #fe_mem_heaprealloc() 
 * on the \c base member.
 *
 * But keep in mind you can use it like the simple buffer+length combination
 * it actually is. An other API requiring an #fe_iov from you does not imply
 * it must be dynamic.
 */
typedef struct {
    char *base;
    size_t len;
} fe_iov;



/*! \brief Deinitializes a #fe_iov (Actually calls #fe_mem_heapfree() on its buffer).
 * To initialize a fe_iov struct, just clear it to zero :
 * \code{.c}
 * fe_iov iov = {0};
 * \endcode
 */
void    fe_iov_deinit(fe_iov *iov);
/*! \brief TODO */
bool    fe_iov_resize(fe_iov *iov, size_t len);
/*! \brief TODO */
static inline bool fe_iov_grow(fe_iov *iov, size_t len) {
    return fe_iov_resize(iov, iov->len+len);
}
/*! \brief TODO */
size_t fe_iov_vprintf(fe_iov *iov, size_t offset, const char *fmt, va_list ap);
/*! \brief TODO */
size_t fe_iov_printf(fe_iov *iov, size_t offset, const char *fmt, ...);

/*! \brief TODO */
static inline char* fe_vasprintf(const char *fmt, va_list ap) {
    fe_iov iov = {0};
    fe_iov_vprintf(&iov, 0, fmt, ap);
    return iov.base;
}
/*! \brief TODO */
static inline char* fe_asprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char *str = fe_vasprintf(fmt, ap);
    va_end(ap);
    return str;
}
/*! \brief \p offset is relative to \p iov. */
void   fe_iov_copy(fe_iov *iov, size_t offset, const fe_iov *src);





/*! \brief TODO */
typedef enum {
    FE_IOV_SUCCESS = 0,
    FE_IOV_PROGRESS_RESOLVING_HOST,
    FE_IOV_FAILED_UNKNOWN,
    FE_IOV_FAILED_SYSERR,
    FE_IOV_FAILED_NO_MEM,
    FE_IOV_FAILED_NO_HOST,
    FE_IOV_FAILED_NO_ENTRY
} fe_iov_code;

typedef enum {
    FE_IOV_STEP_PENDING = 0,
    FE_IOV_STEP_STARTED,
    FE_IOV_STEP_COMPLETED
} fe_iov_step;

typedef struct {
    fe_iov_step step     :  2;
    fe_iov_code current  : 12; /* Valid only when step >= FE_IOV_STEP_STARTED. */
    bool        success  :  1; /* Valid only when step == FE_IOV_STEP_COMPLETED.  */
    bool        exists   :  1; /* Valid only when success and the request is 'exists()'. */
    int         last_error; /* DWORD, GetLastError() on Win32. errno otherwise. */
} fe_iov_status;

/*! \brief TODO */
typedef struct {
    size_t bytes_current; /*!< Downloaded, read or written. */
    size_t bytes_total; /*!< How many are expected to be read/written for this operation.
                             Beware, it may be zero ! (e.g with fe_fd_sync()). */
    fe_iov_status status; /*!< This only gives you a hint about what did go wrong.
    * If you want to build a string to show to the user, you're on your own (any URL or
    * filename is yours to keep track of). */
} fe_iov_progress;


/*! \brief TODO */
/* Anything in there, even the enum's values, may be platform-specific #defines. */
typedef enum {
    FE_FD_READ                     = 1,
    FE_FD_WRITE                    = 1<<1,
    FE_FD_READWRITE                = 1 | 1<<1,
    FE_FD_APPEND                   = 1<<2,
    FE_FD_CREATE                   = 1<<3,
    FE_FD_NONEXISTENT              = 1<<4,
    FE_FD_TRUNCATE                 = 1<<5,
    FE_FD_HINT_RANDOM_ACCESS       = 1<<7,
    FE_FD_HINT_SEQUENTIAL_ACCESS   = 1<<8,
    FE_FD_HINT_SINGLE_ACCESS       = 1<<9,
    FE_FD_HINT_EARLY_ACCESS        = 1<<10,
    FE_FD_HINT_LATE_ACCESS         = 1<<11,
    FE_FD_WIN32_SHARE_READ         = 1<<14,
    FE_FD_WIN32_SHARE_WRITE        = 1<<15,
    FE_FD_WIN32_SHARE_DELETE       = 1<<16,
    FE_FD_WIN32_DELETE_ON_CLOSE    = 1<<17,
    FE_FD_WIN32_WRITE_THROUGH      = 1<<18
    /*
     * FE_FD_LINUX2_DIRECT
     * FE_FD_UNIX_DSYNC
     * FE_FD_UNIX_SYNC
     * FE_FD_LINUX3_TMPFILE (= FILE_DELETE_ON_CLOSE)
     */
} fe_fd_modeflags;

typedef struct {
#ifdef FE_TARGET_WINDOWS
    DWORD desired_access; /* GENERIC_READ | GENERIC_WRITE*/
    DWORD share; /* FILE_SHARE_DELETE |FILE_SHARE_READ |FILE_SHARE_WRITE */
    DWORD creation_disposition;
    /* one of : CREATE_ALWAYS, CREATE_NEW, OPEN_ALWAYS, OPEN_EXISTING, TRUNCATE_EXISTING */
    DWORD attrs_and_flags; /* FILE_FLAG_DELETE_ON_CLOSE, FILE_FLAG_RANDOM_ACCESS, FILE_FLAG_SEQUENTIAL_SCAN, FILE_FLAG_WRITE_THROUGH */
#else
    int flags;
    bool posix_fadv_random     : 1;
    bool posix_fadv_sequential : 1;
    bool posix_fadv_noreuse    : 1;
    bool posix_fadv_willneed   : 1;
    bool posix_fadv_dontneed   : 1;
    /* O_RDONLY | O_WRONLY, O_RDWR, O_APPEND, O_CREAT, O_EXCL, O_TRUNC */
    #ifdef FE_TARGET_ANDROID
        const char *stdio_mode;
    #endif
#endif
} fe_fd_mode;


/*! \brief TODO */
typedef enum {
    FE_FD_SEEK_SET = 0,
    FE_FD_SEEK_CUR = 1,
    FE_FD_SEEK_END = 2
} fe_fd_seek_whence;

#if defined(FE_TARGET_WINDOWS)
    #include <windows.h>
    typedef HANDLE fe_fd;
    #define FE_FD_INVALID_FD INVALID_HANDLE_VALUE
    typedef int64_t fe_fd_offset;
    #define FE_FD_SEEK_SET FILE_BEGIN
    #define FE_FD_SEEK_CUR FILE_CURRENT
    #define FE_FD_SEEK_END FILE_END
#elif defined(FE_TARGET_ANDROID)
    #include <SDL2/SDL.h>
    typedef SDL_RWops *fe_fd;
    #define FE_FD_INVALID_FD NULL
    #define PRIfe_fd "p"
    typedef Sint64 fe_fd_offset;
    #define FE_FD_SEEK_SET RW_SEEK_SET
    #define FE_FD_SEEK_CUR RW_SEEK_CUR
    #define FE_FD_SEEK_END RW_SEEK_END
#else
    #include <sys/types.h>
    typedef off_t fe_fd_offset;
    #ifdef FE_TARGET_EMSCRIPTEN
        typedef struct {
            int unix_fd;
            fe_fpath_emscripten_idb_fpath idb; 
            bool is_idb : 1;
            bool is_readonly : 1;
        } fe_fd;
        #define FE_FD_INVALID_FD ((fe_fd){-1,{NULL, NULL},0,0})
        #define fe_fd_is_valid(fd) ((fd).unix_fd != -1)
    #else
        typedef int fe_fd;
        #define FE_FD_INVALID_FD (-1)
    #endif
    #define FE_FD_SEEK_SET SEEK_SET
    #define FE_FD_SEEK_CUR SEEK_CUR
    #define FE_FD_SEEK_END SEEK_END
#endif

/*! \brief TODO*/
typedef struct {
    void *base;
    size_t len;
    void *view_base;
#ifdef FE_TARGET_WINDOWS
    HANDLE filemapping;
#endif
} fe_filemapview;


typedef struct {
    bool valid                                     : 1;
    bool e_neither_read_nor_write                  : 1;
    bool e_append_without_write                    : 1;
    bool e_truncate_without_write                  : 1;
    bool e_append_and_truncate                     : 1;
    bool w_nonexistent_without_create              : 1;
    bool w_linux_truncate_on_nonexistent           : 1;
    bool w_stdio_create_implies_append_or_truncate : 1;
    bool w_windows_no_actual_append_mode           : 1;
    bool w_windows_filemapping_write_shared        : 1;
    bool w_hint_both_random_and_sequential         : 1;
    bool w_hint_both_early_and_late_access         : 1;
} fe_fd_modeflags_validation;



/*! \brief TODO */
typedef void *fe_iov_promise;

typedef int fe_iov_error;
fe_iov_error   fe_iov_get_last_error(void);
char*          fe_iov_error_str(fe_iov_error err);

#if defined(FE_DEBUG_BUILD) && !defined(FE_IOV_DBG)
#define FE_IOV_DBG
#endif

#ifdef FE_IOV_DBG
typedef struct {
    const void *func_addr;
    const char *func_name;
    fe_iov_error error;
    union {
        const fe_fpath fpath;
        const char *path;
    };
    fe_fd fd;
} fe_iov_dbg_info;
typedef void (*fe_iov_dbg_callback)(const fe_iov_dbg_info *info);
void           fe_iov_set_local_debug_callback (fe_iov_dbg_callback callback);
void           fe_iov_set_global_debug_callback(fe_iov_dbg_callback callback);
#endif

bool           fe_fs_setcwd      (const char *path);
char*          fe_fs_getcwd      (void);
uint64_t       fe_fs_get_mtime   (const fe_fpath fpath);

bool           fe_fs_exists      (const fe_fpath fpath);
fe_iov_promise fe_fs_exists_async(const fe_fpath fpath);
bool           fe_fs_delete      (const fe_fpath fpath);
fe_iov_promise fe_fs_delete_async(const fe_fpath fpath);

bool           fe_wget           (fe_iov *iov, const char *url);
fe_iov_promise fe_wget_async     (fe_iov *iov, const char *url);
bool           fe_fs_load        (fe_iov *iov, const fe_fpath fpath);
fe_iov_promise fe_fs_load_async  (fe_iov *iov, const fe_fpath fpath);
bool           fe_fs_store       (fe_iov *iov, const fe_fpath fpath);
fe_iov_promise fe_fs_store_async (fe_iov *iov, const fe_fpath fpath);

FE_DECL_PURE void fe_fd_modeflags_validate(fe_fd_modeflags_validation *v, fe_fd_modeflags f);
char* fe_fd_modeflags_validation_str(fe_fd_modeflags_validation status);
FE_DECL_PURE void fe_fd_modeflags_compile(fe_fd_mode *m, fe_fd_modeflags f);


/* len should be >= 32. */
void           fe_fd_str(fe_fd fd, char *buf, size_t len);
/* On an Emscripten Wget file, this call blocks until the download is complete.
 * This is most likely not what you want, so you should first download the
 * file using fe_iov_load_wget_async(), handle the promise properly,
 * and save it to MEMFS, then open it on MEMFS. */
fe_fd          fe_fd_open(const fe_fpath fpath, fe_fd_mode mode);
#ifndef        fe_fd_is_valid
#define        fe_fd_is_valid(fd) ((fd) != FE_FD_INVALID_FD)
#endif
bool           fe_fd_mmap(fe_filemapview *v, fe_fd fd, fe_fd_offset offset, size_t len, bool rw);
bool           fe_fd_msync_hint(fe_filemapview *v);
bool           fe_fd_munmap(fe_filemapview *v);
fe_fd_offset   fe_fd_seek(fe_fd fd, fe_fd_offset offset, fe_fd_seek_whence whence);
#define        fe_fd_tell(fd) (fe_fd_seek(fd, 0, FE_FD_SEEK_CUR))
fe_fd_offset   fe_fd_size(fe_fd fd);
ssize_t        fe_fd_read(fe_fd fd, void *buf, size_t len);
ssize_t        fe_fd_write(fe_fd fd, const void *buf, size_t len);
ssize_t        fe_fd_readv(fe_fd fd, fe_iov *iov_array, size_t iov_count);
ssize_t        fe_fd_writev(fe_fd fd, const fe_iov *iov_array, size_t iov_count);
ssize_t        fe_fd_preadv(fe_fd fd, fe_iov *iov_array, size_t iov_count, fe_fd_offset offset);
ssize_t        fe_fd_pwritev(fe_fd fd, const fe_iov *iov_array, size_t iov_count, fe_fd_offset offset);
bool           fe_fd_sync(fe_fd fd);
bool           fe_fd_truncate(fe_fd fd, size_t len);
bool           fe_fd_close(fe_fd fd);

bool fe_iov_promise_poll(fe_iov_promise p, fe_iov_progress *st);
bool fe_iov_promise_wait(fe_iov_promise p, fe_iov_progress *st, int timeout_milliseconds);
void fe_iov_promise_cancel(fe_iov_promise p);

/*! @} */

struct fe_iov_promise_struct {
    fe_iov_progress progress;
};

#endif /* FE_IOV_H */
