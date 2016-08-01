
size_t fe_printf_get_len(const char *fmt, ...);

/* This is a POD which members are guaranteed to stay the same. */
typedef struct {
    void *base;
    size_t len;
} fe_iov;

#define FE_IOV_INITIALIZER(data,len) {data, len}
void    fe_iov_init(fe_iov *iov); /* known to be equivalent to clearing it to 0. */
void    fe_iov_deinit(fe_iov *iov); /* known to be equivalent to calling fe_mem_heapfree() on the base pointer. */
void    fe_iov_resize(fe_iov *iov, size_t len);
void    fe_iov_grow(fe_iov *iov, size_t len);
int     fe_iov_printf_offset(fe_iov *iov, size_t offset, const char *fmt, ...);
int     fe_iov_printf_append(fe_iov *iov, const char *fmt, ...);
void    fe_iov_write_offset(fe_iov *iov, size_t offset, const char *fmt, ...);
void    fe_iov_write_append(fe_iov *iov, const char *fmt, ...);

/* 
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
 */
typedef enum {
    FE_IOV_ROOTDIR_ANDROID_INTERNAL_STORAGE      = 1<<0, /*!< Applies only to Android. */
    FE_IOV_ROOTDIR_ANDROID_EXTERNAL_STORAGE      = 1<<1, /*!< Applies only to Android. */
    FE_IOV_ROOTDIR_WINRT_PATH_LOCAL_FOLDER       = 1<<2, /*!< Applies only to WinRT. */
    FE_IOV_ROOTDIR_WINRT_PATH_INSTALLED_LOCATION = 1<<3, /*!< Applies only to WinRT. */
    FE_IOV_ROOTDIR_SDL2_GETBASEPATH              = 1<<4, /*!< Overrides any other flag from the above. */
    FE_IOV_ROOTDIR_SDL2_GETPREFPATH              = 1<<5  /*!< Overrides any other flag from the above. */
} fe_iov_rootdir;
fe_iov_promise fe_iov_load_wget(fe_iov *iov, const char *url);
fe_iov_promise fe_iov_load_file(fe_iov *iov, const char *filename, fe_iov_rootdir rd);
fe_iov_promise fe_iov_load_persistent(fe_iov *iov, const char *idb_name, const char *filename, fe_iov_rootdir rd);
fe_iov_promise fe_iov_load_res(fe_iov *iov, const char *url, const char *filename, fe_iov_rootdir rd);
fe_iov_promise fe_iov_store_file(fe_iov *iov, const char *filename, fe_iov_rootdir rd);
fe_iov_promise fe_iov_store_persistent(fe_iov *iov, const char *idb_name, const char *filename, fe_iov_rootdir rd);

/* The fe_fd API's goal is to achieve efficient I/O on targets other than Emscripten (while still working on Emscripten, though).
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
 * -> Files are not guaranteed to be overwritten unless you call fe_fd_sync(). fe_fd_munmap() does not do implicitly do it.
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
 */
/* Anything in there, even the enum's values, may be platform-specific #defines. */
typedef enum {
    FE_FD_OPEN_READONLY,
    FE_FD_OPEN_WRITEONLY,
    FE_FD_OPEN_READWRITE,
    FE_FD_OPEN_APPEND,
    FE_FD_OPEN_ENSURE_NONEXISTENT,
    FE_FD_OPEN_TRUNCATE,
    FE_FD_OPEN_SPARSE_ACCESS_HINT,
    FE_FD_OPEN_SEQUENTIAL_ACCESS_HINT,
    FE_FD_OPEN_LAZY_DOWNLOAD
} fe_fd_flags;
typedef enum {
    FE_FD_SEEK_SET,
    FE_FD_SEEK_CUR,
    FE_FD_SEEK_END
} fe_fd_seek_whence;
typedef off_t fe_fd_offset;
fe_fd         fe_fd_open_file(const char *filename, fe_iov_rootdir rd, fe_fd_flags flags);
fe_fd         fe_fd_open_persistent(const char *idb_name, const char *filename, fe_iov_rootdir rd, fe_fd_flags flags);
fe_fd         fe_fd_open_res(const char *filename, fe_iov_rootdir rd, fe_fd_flags flags);
fe_fd_promise fe_fd_get_download_promise(fe_fd fd);
bool          fe_fd_is_valid(fe_fd fd);
size_t        fe_fd_get_len(fe_fd fd);
void*         fe_fd_mmap(fe_fd fd, fe_fd_offset offset, size_t len, bool rw);
void          fe_fd_munmap(void *addr, size_t len);
fe_fd_offset  fe_fd_seek(fe_fd fd, fe_fd_offset offset, fe_fd_seek_whence whence);
#define       fe_fd_tell(fd) fe_fd_seek(fd, 0, FE_FD_SEEK_CUR)
ssize_t       fe_fd_read(fe_fd fd, void *buf, size_t len);
ssize_t       fe_fd_write(fe_fd fd, const void *buf, size_t len);
ssize_t       fe_fd_readv(fe_fd fd, fe_iov *iov_array, size_t iov_count);
ssize_t       fe_fd_writev(fe_fd fd, const fe_iov *iov_array, size_t iov_count);
ssize_t       fe_fd_preadv(fe_fd fd, fe_iov *iov_array, size_t iov_count, fe_fd_offset offset);
ssize_t       fe_fd_pwritev(fe_fd fd, const fe_iov *iov_array, size_t iov_count, fe_fd_offset offset);
bool          fe_fd_sync(fe_fd fd);
bool          fe_fd_truncate(fe_fd fd, size_t len);
void          fe_fd_close(fe_fd fd);
/* Should there be an async version of the API ??
 * We can do async reads with OVERLAPPED.
 * But sometimes it's not worth the trouble.
 * So we could be able to do both sync and async.
 *
 * Conclusion : An async version of this API should come out later.
 */

/* The 3 promise states :
 * - Pending;
 * - Started;
 * - Completed : either fulfilled or broken. */
typedef enum {
    FE_IOV_PROMISE_FULFILLED = 0,
    FE_IOV_PROMISE_PENDING,
    FE_IOV_PROMISE_STARTED_RESOLVING_HOST,
    FE_IOV_PROMISE_BROKEN_NO_HOST,
    FE_IOV_PROMISE_BROKEN_NO_ENTRY
} fe_iov_promise_status;

typedef struct {
    size_t bytes_current; /*!< Downloaded, read or written. */
    size_t bytes_total; /*!< How many are expected to be read/written for this operation.
                             Beware, it may be zero ! (e.g with fe_fd_sync()). */
    fe_iov_promise_status_detail status; /*!< This only gives you a hint about what did go wrong.
    * If you want to build a string to show to the user, you're on your own (any URL or
    * filename is yours to keep track of). */
} fe_iov_promise_state;

typedef void (*fe_iov_promise_completion_routine)(const fe_iov_promise_state *st, void *userdata);
bool fe_iov_promise_poll(fe_iov_promise p, fe_iov_promise_state *st);
bool fe_iov_promise_wait(fe_iov_promise p, fe_iov_promise_state *st, int timeout_milliseconds);
void fe_iov_promise_notify(fe_iov_promise p, fe_iov_promise_completion_routine callback, void *userdata);
void fe_iov_promise_cancel(fe_iov_promise p);

/* Both #fe_iov_promise_poll and #fe_iov_promise_wait() return whether you should try again 
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
 */


typedef enum {
    FE_3DOBJ_PROMISE_FULFILLED = 0,
    FE_3DOBJ_PROMISE_STARTED_TRIANGLES,
    FE_3DOBJ_PROMISE_STARTED_NORMALS
};
typedef struct {
    fe_3dobj_promise_status status;
    size_t triangles_current;
    size_t triangles_total;
} fe_3dobj_promise_state;
fe_3dobj_promise fe_3dobj_load(fe_3dobj *obj, const fe_iov *iov);
bool fe_3dobj_promise_poll(fe_3dobj_promise p, fe_3dobj_promise_state *st);
bool fe_3dobj_promise_wait_change(fe_3dobj_promise p, fe_3dobj_promise_state *st, int timeout_ms);
bool fe_3dobj_promise_wait_completion(fe_3dobj_promise p, fe_3dobj_promise_state *st, int timeout_ms);


/* Typical use case: */

cj_iov_load_res() {
    fe_iov_load_res(...);
}
static my_obj, my_iov = {0};
static my_iov_promise, my_obj_promise;
(*ontick)();
onstart() {
    my_iov_promise = cj_iov_load_res(&my_iov, CJ_RES_3DOBJ_shukuen);
    ontick = ontick_loadiov;
}
ontick_playgame() {}
ontick_loadobj() {
    fe_3dobj_promise_state objst;
    fe_3dobj_promise_poll_change(my_obj_promise, &objst);
    if(objst.status == FE_3DOBJ_PROMISE_FULFILLED)
        ontick = ontick_playgame;
    print(&objst);
}
ontick_loadiov() {
    fe_iov_promise_state iovst;
    fe_iov_promise_poll_change(my_iov_promise, &iovst);
    if(iovst.status == FE_IOV_PROMISE_FULFILLED) {
        my_obj_promise = fe_3dobj_load(&my_obj, &my_iov);
        ontick = ontick_loadobj;
    }
    print(&iovst);
}

