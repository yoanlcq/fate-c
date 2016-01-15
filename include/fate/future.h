/*! \file future.h
 *  \brief Interface to F.A.T.E's promise subsystem.
 *
 * On startup, F.A.T.E launches a pool of N threads, where N can be changed
 * at runtime, initially being the number of CPU cores.
 *
 * The global state holds (doubly ?) linked lists of tasks.
 * Tasks are continuously added, waiting to be completed. No task depends on
 * the completion of another.
 * A task may be added with a priority and an estimated completion time; those
 * attributes need not be precise as they are only a hint.
 *
 * Each task list corresponds more or less to a category. Threads subscribe
 * to one or more lists depending on their privileges.
 * (Typically, only the main thread will subscribe to the "Window Events list"
 * just like the only thread responsible for the OpenGL context will subscribe
 * to the "Video list").
 *
 * The current task lists are :
 * - <span style="color:cyan;">The Window list;</span>
 * - <span style="color:orange;">The Video list;</span>
 * - <span style="color:yellow;">The Audio list;</span>
 * - <span style="color:green;">The File IO list;</span>
 * - <span style="color:red;">The Network IO list;</span>
 * - <span style="color:blue;">The Compute list;</span>
 * - <span style="color:magenta;">The Miscellaneous list;</span>
 *
 * Each thread's job is to :
 * - Wait until a list it subscribed to is non-empty;
 * - Pop the task with highest priority;
 * - Execute it;
 * - Go back to step 1.
 *
 * With Emscripten, things behave a bit differently. There is a single
 * thread, which makes asynchronous calls to simulate a multi-threaded
 * behaviour.
 *
 */



#ifndef FATE_FUTURE_H
#define FATE_FUTURE_H

#include <stdbool.h>

/*
typedef void (*em_str_callback_func)(const char *);
typedef void (*em_async_wget2_data_onload_func)(void *arg, void *buf, unsigned *size);
typedef void (*em_async_wget2_data_onerror_func)(void *arg, int http_code, const char *description)
typedef void (*em_async_wget2_data_onprogress_func)(void *arg, int bytes_loaded, int total_size);

void emscripten_wget(const char* url, const char* file);
void emscripten_async_wget(const char* url, const char* file, em_str_callback_func onload, em_str_callback_func onerror);
int emscripten_async_wget2_data(const char* url, const char* requesttype, const char* param, void *arg, int free, em_async_wget2_data_onload_func onload, em_async_wget2_data_onerror_func onerror, em_async_wget2_data_onprogress_func onprogress)
void emscripten_async_wget2_abort(int handle);

typedef void (*em_async_wget_onload_func)(void *arg, void *buf, int size);
typedef void (*em_arg_callback_func)(void *arg);
void emscripten_idb_async_load(const char *db_name, const char *file_id, void* arg, em_async_wget_onload_func onload, em_arg_callback_func onerror)
void emscripten_idb_async_store(const char *db_name, const char *file_id, void* ptr, int num, void* arg, em_arg_callback_func onstore, em_arg_callback_func onerror);
*/

/*! \brief Object holding both a promise and its future. 
 */
struct fate_future;
/*! \brief Object holding both a promise and its future. 
 *  \see struct fate_future
 */
typedef struct fate_future fate_future;

/*! \brief Error codes for broken promises. 
 */
enum fate_promise_error {
    FATE_PROMISE_ERR_NONE = 0, /*!< No error. */
    FATE_PROMISE_ERR_UNKNOWN = 1 /*!< Unknown error. */
};
/*! \brief Error codes for broken promises. 
 */
typedef enum fate_promise_error fate_promise_error;

/*! \brief Definition for an asynchronous task given to #fate_promise_task.
 *
 * The task must return its result as a dynamically allocated buffer (like with malloc()) and
 * set the value pointed to by \p result_size to its size in bytes.
 *
 * NULL is not a legitimate return value since it is reserved to break the promise.<br>
 * If the function truly should return nothing on success, it should return a dynamically allocated
 * empty string instead, for example.
 * 
 * The task is given a #fate_future object so it can (and should) :
 * - Call #fate_promise_set_progress() on it.
 * - Call #fate_promise_set_error_str() on it (then please return NULL).
 * 
 * When the task function returns, its promise's progress is automatically set to 1.0.<br>
 * If NULL was returned, the value at \p result_size is automatically set to 0. 
 *
 * \param future A #fate_future object so the task can indicate its progress or set its error.
 * \param arg A user parameter given to fate_promise_task.
 * \param result_size Must be set to indicate the returned buffer's size in bytes.
 * \return A dynamically-allocated buffer (like with malloc()).
 * \see fate_promise_task
 */
typedef void* (*fate_taskfunc)(fate_future *future, void *arg, size_t *result_size);

/*! \brief A type for holding a file interaction mode.
 *
 * Valid values are :
 * - "x"   - Test existence (get the result with #fate_future_get_i()).
 * - "d"   - Delete the file (get the result with #fate_future_get_i()).
 * - "r"   - Open file for reading.
 * - "r+"  - Open file for reading and writing.
 * - "w"   - Open file for writing.
 *             The file is created if it does not exist or truncated if it exists.
 * - "wx"  - Like "w" but fails if path exists.
 * - "w+"  - Like "w" but also allow reading.
 * - "wx+" - Like "w+" but fails if path exists.
 * - "a"   - Open file for appending. The file is created if it does not exist.
 * - "ax"  - Like "a" but fails if path exists.
 * - "a+"  - Open file for reading and appending.
 *             The file is created if it does not exist.
 * - "ax+" - Like "a+" but fails if path exists.
 *
 * On all modes, a "b" can be appended to indicate a binary stream.
 * It is ignored with the "d" and "x" modes.
 *
 * \see fate_promise_file
 */
typedef const char* fate_file_mode_string;

/*! \brief A type for indicating where to load/store a file. 
 *
 *  \see fate_promise_file */
enum fate_storage {
    FATE_STORAGE_FILESYSTEM = 1, /*!< The file system, no matter the platform.
    * <br>On Emscripten, it is MEMFS, which means the files must be packaged 
    * with emcc's --preload-file option. */
    FATE_STORAGE_WEB = 2, /*!< The Web, no matter the platform, through HTTP with GET and POST.  
    * <br>There's no way to choose a port other than the default, 
    * since Emscripten doesn't allow it.<br>
    * File interaction via FTP is also possible with #fate_promise_file_ftp(). */
    FATE_STORAGE_RESOURCE = 3, /*!< The resource storage (usually for asset data).
    - On Emscripten, it is the application's server.
    - On mobile devices, it is the app's resource directory.
    - Else, it is the file system. */
    FATE_STORAGE_PERSISTENT = 4, /*!< The platform's persistent storage (usually for user-specific data).
    - On Emscripten, it is IndexedDB.
    - On mobile devices, it is the platform-specific persistent storage.
    - Else, it is the file system. */
};
/*! \brief A type for indicating where to load/store a file. 
 *  \see enum fate_storage */
typedef enum fate_storage fate_storage;





/* STARTING PROMISES */



/*! \brief Creates a file promise for opening, checking or deleting a file.
 * 
 * \p timeout_ms should indicate the expected response time from the server, in millseconds.<br>
 * Setting it to -1 means the promise will keep waiting for the file.<br>
 * Setting it to 0 is recommended and means the promise will reasonably keep 
 * waiting for the file, but fail with an error it the wait is too long.
 *
 * A copy of \p path_or_url is kept.
 *
 * Please refer to the links below.
 *
 * \see fate_future
 * \see fate_storage
 * \see fate_file_mode_string
 * \see fate_future_get_file 
 */
void fate_promise_file(fate_future *future, 
                       int timeout_ms, 
                       fate_storage storage, 
                       const char *path_or_url, 
                       fate_file_mode_string mode);


/*! \brief Creates a file promise for opening, checking or deleting a file though FTP.
 * 
 * The promise connects to a FTP server, checks/downloads/truncates a single file, then disconnects.
 * 
 * If the file is opened for writing, calling #fate_future_file_promise_commit() will connect to the
 * server again, upload the file, then disconnect.<br>
 * There's no way to get directories this way, but the content provider can just pack them in
 * an archive.
 *
 * \p timeout_ms should indicate the expected response time from the server, in milliseconds.<br>
 * Setting it to -1 means the promise will keep waiting for the file.<br>
 * Setting it to 0 is recommended and means the promise will reasonably keep 
 * waiting for the file, but fail with an error it the wait is too long.
 *
 * A copy of all of the string parameters is kept, expect \p mode.
 *
 * port should be set to FATE_FTP_DEFAULT_PORT which evaluates to 21.
 *
 * \see fate_future
 * \see fate_file_mode_string
 * \see fate_future_get_file 
 */
void fate_promise_file_ftp(fate_future *future, 
                           int timeout_ms,
                           const char *url,
                           unsigned short port,
                           const char *login,
                           const char *passwd,
                           const char *path,
                           fate_file_mode_string mode);

/*! \brief Creates a task promise, for... executing a task (Duh !).
 * 
 * Please refer to the links below.
 * \see fate_future
 * \see fate_taskfunc
 */
void fate_promise_task(fate_future *future, fate_taskfunc task, void *arg);




/* PROMISE UTILITY FUNCTIONS */



#ifdef __EMSCRIPTEN__
#define fate_sleep_with_yield emscripten_sleep_with_yield
#else
/*! \brief Yields some time for promises to run.
 *
 * It is possible to achieve an active wait by calling repeatedly 
 * #fate_sleep_with_yield() and #fate_promise_get_progress(). */
void fate_sleep_with_yield(unsigned ms);
#endif

/*! \brief Gets a promise's progress, between 0 and 1. 
 * 
 */
float fate_promise_get_progress(fate_future *future);

/*! \brief Gets precise values used to compute a file promise's progress. 
 *
 * Of course, \p bytes_read is guaranteed to be less or equal than \p bytes_total. */
void fate_promise_get_file_progress(fate_future *future, unsigned *bytes_read, unsigned *bytes_total);

/*! \brief Gets a string describing a promise's progress, to be freed by the caller.
 * 
 * It atomically returns a dynamically-allocated copy of the promise's internal progress string.
 *
 * It is meaningful because it allows an online file promise to indicate the connection status while
 * the promise's progress is 0.
 */
char *fate_promise_get_progress_str(fate_future *future);


/* SETTING THE PROMISE STATE FROM WITHIN A TASK */


/*! \brief Used by tasks to set their promise's progress. 
 * 
 * Should only be called by routines given to fate_promise_task(). 
 * 
 * Nothing is done if \p progress is not within the interval [0, 1[. */
void fate_promise_set_progress(fate_future *future, float progress);

/*! \brief Used by tasks to set their promise's progress string.
 *
 * The string is not copied. It is the task's responsibility to keep the pointer alive.
 */
void fate_promise_set_progress_str(fate_future *future, const char *str);


/*! \brief Used by tasks to set their promise's error string. 
 * 
 * Should only be called by routines given to fate_promise_task(), 
 * which should then return NULL. 
 *
 * If the error code is currently zero, it is set to FATE_PROMISE_ERR_UNKNOWN. */
void fate_promise_set_error_str(fate_future *future, const char *error);

/*! \brief Used by tasks to set their promise's error code. 
 * 
 * Should only be called by routines given to fate_promise_task(), 
 * which should then return NULL. */
void fate_promise_set_error(fate_future *future, fate_promise_error e);





/* GETTING THE FUTURE */



/*! \brief Waits for the future, and returns a string 
 *         describing the future's last error, or the empty string if there's none. */
const char *fate_future_get_error_str(fate_future *future);

/*! \brief Waits for the future, and
 *         returns an integer describing the future's last error,
 *         or 0 if there's none. 
 * 
 * Not all errors can be described by an error code.<br>
 * This function is only provided for callers to reliably
 * handle common error cases. 
 */
fate_promise_error fate_future_get_error(fate_future *future);


/*! \brief Waits for the file future, and returns an opened file, or NULL if there was an error.
 *
 * If the file handle is NULL, the promise was broken, and the last error 
 * can be described by #fate_future_get_error_str().
 * 
 * Replace calls to fflush() on the file by #fate_future_file_promise_commit().<br>
 * Replace calls to fclose() on the file by #fate_future_close(). */
FILE *fate_future_get_file(fate_future *future);

/*! \brief Waits for the task future, and returns a buffer, or NULL if there was an error.
 * 
 * The buffer's size is indicated in \p result_size.
 * 
 * If the buffer is NULL (or \p result_size points to 0), the promise was broken,
 * and the last error can be described by #fate_future_get_error_str().
 *
 * Do not free() the buffer. Use #fate_future_close() instead. */
void *fate_future_get_v(fate_future *future, size_t *result_size);

/*! \brief Waits for the future, and returns an integer or boolean, less than 0 if there was an error.
 *
 * Currently only available to specific file futures (like "testing existence" 
 * or "delete" promises).
 *  
 * If the value is less than 0, the promise was broken, and the last
 * error can be described by #fate_future_get_error_str(). */
int fate_future_get_i(fate_future *future);

/*! \brief Promises committing a file future to the storage (by uploading it if needed).
 * 
 * \p future becomes a regular promise again, with a progress set to 0.<br> 
 * The file handle previously returned by #fate_future_get_file() must be considered invalid.<br>
 * The caller must get a new one by calling #fate_future_get_file() again (i.e waiting for the future).
 *
 * If the future was not available or the future is a read-only file,
 * this function is a no-op since there's nothing to do. 
 *
 * File futures have a "dirty flag" indicating whether committing is necessary.<br>
 * This flag is initially raised.<br>
 * If the "dirty flag" is not raised, this function does nothing.<br>
 * Otherwise, this function sets the "dirty flag" to 'false' and commits data.<br>
 * The caller is responsible for raising it again if needed, via #fate_future_file_set_dirty().
 */
void fate_future_file_promise_commit(fate_future *future);

/*! \brief Gets a file future's dirty flag.
 * \see fate_future_file_promise_commit(). */
bool fate_future_file_get_dirty(fate_future *future);

/*! \brief Sets a file future's dirty flag.
 * \see fate_future_file_promise_commit(). */
void fate_future_file_set_dirty(fate_future *future, bool dirty);


/*! \brief Aborts a promise or releases its future.
 * 
 * If the future is not available yet, the promise is cancelled 
 * and resources are released.<br>
 * Else, the future's resources are released.
 * (This is where free() and/or fclose() are called).
 * 
 * After that call, the #fate_future object can be reused for new promises.
 * 
 * For file futures, an implicit fate_future_file_promise_commit() 
 * is performed synchronously before releasing resources.<br>
 * Therefore, it is recommended, for online files, to first call
 * #fate_future_file_promise_commit() in order to handle the upload's progress. */
void fate_future_close(fate_future *future);


#endif /* FATE_FUTURE_H */
