#ifndef FATE_FUTURE_H
#define FATE_FUTURE_H

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


/*
 * On startup, F.A.T.E launches a pool of N threads, where N can be changed
 * at runtime, initially being the number of CPU cores.
 *
 * The global state holds (doubly ?) linked lists of tasks, which 
 * are continuously filled as the program runs.
 * A task may be added with a priority and an estimated completion time, which
 * need not be precise as they are only a hint.
 *
 * Each task list corresponds more or less to a category. Threads subscribe
 * to one or more of the list depending on their privileges.
 * (Typically, only the main thread will subscribe to the "Window Events list"
 * just like the only thread responsible for the OpenGL context will subscribe
 * to the "Video list").
 *
 * The current task lists are :
 * - The Video list;
 * - The Audio list;
 * - The Window events list;
 * - The File IO list;
 * - The Network IO list;
 * - The Compute list;
 * - The Miscellaneous list;
 *
 * Each thread's job is to :
 * - Wait until the list is non-empty;
 * - Pop the task with highest priority;
 * - Execute it;
 * - Repeat.
 *
 * With Emscripten, things behave a bit differently. There is a single
 * thread, which makes asynchronous calls to simulate a multi-threaded
 * behaviour.
 *
 */


struct fate_future_file {
    FILE *file;
    uint32_t bytes_read, bytes_total;
};

struct fate_future_call {
    void *result;
    FILE (*task)(fate_future *f, void *arg);
    float progress;
};

union fate_futureobj {
    struct fate_future_file file;
    struct fate_future_call call;
};

struct fate_future {
    union fate_futureobj obj;
    const char *error_string;
};
typedef struct fate_future fate_future;

/* Start a promise. */
/*
 * On file promises, mode can be one of :
 *
 * "d"   - Delete the file.
 * "x"   - Test existence.
 * "r"   - Open file for reading.
 * "r+"  - Open file for reading and writing.
 * "w"   - Open file for writing.
 * "wx"  - Like "w" but fails if path exists.
 * "w+"  - Open file for reading and writing. 
 *         The file is created if it does not exist or truncated if it exists.
 * "wx+" - Like "w+" but fails if path exists.
 * "a"   - Open file for appending. The file is created if it does not exist.
 * "ax"  - Like "a" but fails if path exists.
 * "a+"  - Open file for reading and appending.
 *         The file is created if it does not exist.
 * "ax+" - Like "a+" but fails if path exists.
 */
/* resource() has one of the two behaviours :
 * - On Emscripten, the file is loaded/stored from the server. 
 * - Else, the file is opened just like a regular file. */
void fate_promise_file_resource(fate_future *f, const char *path, const char *mode);
/* persistent() has one of the two behaviours :
 * - On Emscripten, the file is loaded/stored to IndexedDB, through IDBFS
 * - Else, the file is opened just like a regular file. */
void fate_promise_file_persistent(fate_future *f, const char *path, const char *mode);
/* online() will always fetch a file from a url or POST it. 
 * This is useful when not on Emscripten.*/
void fate_promise_file_online(fate_future *f, const char *url, const char *mode);
/* the 'task' must store its result in an in-memory file (fmemopen()...) */
void fate_promise_call(fate_future *f, FILE (*task)(fate_future *f, void *arg), void *arg);

#ifdef __EMSCRIPTEN__
#define fate_sleep_with_yield emscripten_sleep_with_yield
#else
/* Yield some time for async requests. */
void fate_sleep_with_yield(unsigned ms);
#endif

/* Set a promise's progress. Should only be called by routines given to async_call(). */
void fate_promise_setprogress(fate_future *f);

/* Works for all promises. Returns less than 0.0f in case of error.*/
float fate_promise_getprogress(fate_future *f);

/* total points to 0 in case of error. */
void fate_promise_getfileprogress(fate_future *f, uint32_t *nbytes, uint32_t *total);

/* Wait for a promise to finish or detect errors.
 * The returned value is used for error detection since the return value
 * of a task might legitimately be NULL.
 * Supposedly better than active waiting with yield() and getprogress(). 
 * It returns the result of an async call, or NULL for file promises. */
bool fate_future_wait(fate_future *f);

/* Does not perform an implicit wait(). */
const char *fate_future_geterror(fate_future *f);

/* Performs an implicit wait(). */
FILE *fate_future_get(fate_future *f);

/* Available only to file promises. Waits until data has completely been committed. 
 * For read-only files or files that are still downloading, this is a no-op since there's nothing to do. */
void fate_future_commit(fate_future *f);

/* Applies to all promises.
 * If the file is still downloading, aborts the request.
 * Else, performs an implicit commit() and releases resources. */
void fate_future_close(fate_future *f);


#endif /* FATE_FUTURE_H */
