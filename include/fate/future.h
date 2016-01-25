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

/*! \file fate/future.h
 *  \brief Interface to F.A.T.E's promise subsystem.
 *
 * On startup, F.A.T.E launches a pool of N threads, where N can be changed
 * at runtime, but initially being the number of CPU cores.
 *
 * The global state holds (doubly ?) linked lists of tasks, called <b>Task lists</b>.<br>
 * Tasks are continuously added, waiting to be completed. Some tasks may depend on
 * the completion of other tasks, so it might look like a dependency graph.
 *
 * As described in Intel's Dont Dread Threads, a task is the smallest unit of
 * work in the program's pipeline.<br>
 * That means, once selected, a task keeps its thread busy until it is completed.<br>
 * To improve load balancing, "rather heavy tasks" should actually be split into small tasks.
 * This way, promises are easier to cancel.
 *
 * For example, starting a file promise does not internally push a big "download file" task.<br>
 * Instead, it pushes several small "download a piece of that file" tasks, each dependant from the previous.
 *
 * A task may be given hints, such as a priority and an estimated completion time.<br>
 * Of course, those hints need not be precise, but they help balancing load between threads properly.
 *
 * Each task list plays a <b>role</b> in the program's execution. Threads subscribe
 * to one or more lists depending on their privileges.<br>
 * (Typically, only the main thread will subscribe to the "Window Events list"
 * just like the only thread responsible for the OpenGL context will subscribe
 * to the "Video list").<br>
 * See #fate_task_role to know more about this.
 * 
 * Each thread's job is to :
 * - Choose a non-empty list it subscribed to (waiting if necessary);
 * - Atomically pop the task with highest priority from that list;
 * - Execute the task;
 * - Go back to the first step.
 *
 * If profiling is enabled, the thread reports statistics, such as, how much time it has spent waiting,
 * the number of times it has waited, how much time it has spent executing each task,
 * which role that task was in, etc.
 *
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


/*! \brief Task categories, used for scheduling.
 */
enum fate_task_role {
    FATE_TASK_MISC = 0, /*!< Miscellaneous.<br>
    * This role's color for visual profiling is <span style="color:magenta;">Magenta</span>.<br>
    * Any task that doesn't neatly fit the other categories should run under this one. */
    FATE_TASK_WINDOW = 1, /*!< Only the thread(s) that can manage windows 
    * and window events will pick tasks under that role.<br>
    * Chances are it is the main thread.<br>
    * This role's color for visual profiling is <span style="color:cyan;">Cyan</span>.<br>
    */
    FATE_TASK_VIDEO = 2, /*!< Only the thread(s) responsible for the 
    * OpenGL (or other) rendering context(s) will pick tasks under that role.<br>
    * This role's color for visual profiling is <span style="color:orange;">Orange</span>.<br>
    */
    FATE_TASK_AUDIO = 3, /*!< Only the thread(s) responsible for the 
    * OpenAL (or other) audio context(s) will pick tasks under that role.<br>
    * This role's color for visual profiling is <span style="color:yellow;">Yellow</span>.<br>
    */
    FATE_TASK_FILEIO = 4, /*!< Local file input/output.<br>
    * This role's color for visual profiling is <span style="color:green;">Green</span>.<br>
    * Any thread can pick tasks under that role, 
    * with more or less probability depending on its responsibilities. */
    FATE_TASK_NETWORKIO = 5, /*!< Network input/output.<br>
    * This role's color for visual profiling is <span style="color:red;">Red</span>.<br>
    * Any thread can pick tasks under that role, 
    * with more or less probability depending on its responsibilities. */
    FATE_TASK_COMPUTE = 6 /*!< Any thread can pick tasks under that role, 
    * with more or less probability depending on its responsibilities.
    * <br>This is quite like FATE_TASK_MISC, except the task is rather expected to actually be a computation.<br>
    * This role's color for visual profiling is <span style="color:blue;">Blue</span>.<br>
    */
};
/*! \brief Task categories, used for scheduling.
 */
typedef enum fate_task_role fate_task_role;


/*! \brief Definition for an asynchronous task given to #fate_promise_task.
 *
 * The task must return its result as a dynamically allocated buffer (like with malloc()) or NULL and
 * set the value pointed to by \p result_size to its size in bytes.
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
 * \return NULL or a dynamically-allocated buffer (like with malloc()).
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
 * Internally, this function adds one or more tasks to the "File IO" 
 * or "Network IO" task lists (see #fate_task_role for more info).
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
 * Internally, this function adds one or more tasks to the "File IO" 
 * or "Network IO" task lists (see #fate_task_role for more info).
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
void fate_promise_task(fate_future *future, fate_task_role role, fate_taskfunc task, void *arg);




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
 * Do not fflush() the file. Use #fate_future_file_promise_commit() instead.<br>
 * Do not fclose() the file. Use #fate_future_close() instead. 
 */
FILE *fate_future_get_file(fate_future *future);



/*! \brief Waits for the task future, and returns its buffer result.
 * 
 * The buffer's size is indicated in \p result_size.
 * Since the buffer can legitimately be NULL, the true way to check for errors
 * is to call #fate_future_get_error_str().
 *
 * Do not free() the buffer. Use #fate_future_close() instead. 
 */
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
