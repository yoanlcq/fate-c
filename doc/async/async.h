#ifndef FATE_ASYNC_H
#define FATE_ASYNC_H

typedef void (*em_str_callback_func)(const char *);
typedef void (*em_async_wget2_data_onload_func)(void *arg, void *buf, unsigned *size);
typedef void (*em_async_wget2_data_onerror_func)(void *arg, int http_code, const char *description)
typedef void (*em_async_wget2_data_onprogress_func)(void *arg, int bytes_loaded, int total_size);

void emscripten_wget(const char* url, const char* file);
void emscripten_async_wget(const char* url, const char* file, em_str_callback_func onload, em_str_callback_func onerror);
/* requesttype is POST or GET. params are given to the POST request. arg is given to callbacks. */
int emscripten_async_wget2_data(const char* url, const char* requesttype, const char* param, void *arg, int free, em_async_wget2_data_onload_func onload, em_async_wget2_data_onerror_func onerror, em_async_wget2_data_onprogress_func onprogress)
void emscripten_async_wget2_abort(int handle);

typedef void (*em_async_wget_onload_func)(void *arg, void *buf, int size);
typedef void (*em_arg_callback_func)(void *arg);
void emscripten_idb_async_load(const char *db_name, const char *file_id, void* arg, em_async_wget_onload_func onload, em_arg_callback_func onerror)
void emscripten_idb_async_store(const char *db_name, const char *file_id, void* ptr, int num, void* arg, em_arg_callback_func onstore, em_arg_callback_func onerror);



struct fate_async_file {
    FILE *file;
    uint32_t bytes_read, bytes_total;
};

struct fate_async_call {
    void *result;
    void (*task)(void *arg, float *progress);
    float progress;
};

union fate_asyncobj {
    struct fate_async_file file;
    struct fate_async_call call;
};

struct fate_async {
    union fate_asyncobj obj;
    const char *error_string;
};
typedef struct fate_async fate_async;

/* Start a promise. */
void fate_async_file_url(fate_async *a, const char *url, const char *mode);
void fate_async_file(fate_async *a, const char *path, const char *mode);
void fate_async_call(fate_async *a, void (*task)(void *arg, float *progress), void *arg);

/* Yield some time for async requests.
 * The first parameter is only a hint and might not be used. It can be NULL. */
void fate_async_yield(fate_async *a, unsigned ms);

/* Works for all promises. Returns less than 0.0f in case of error.*/
float fate_async_getprogress(fate_async *a);

/* total points to 0 in case of error. */
void fate_async_getfileprogress(fate_async *a, uint32_t *nbytes, uint32_t *total);

/* Wait for a promise to finish or detect errors.
 * Supposedly better than active waiting with yield() and getprogress(). 
 * It returns the result of an async call, or NULL for file promises. */
bool fate_async_wait(fate_async *a);

/* Does not perform an implicit wait(). */
const char *fate_async_geterror(fate_async *a);

/* Performs an implicit wait(). Returns the result of a 'call' promise. */
void *fate_async_getresult(fate_async *a);

/* Performs an implicit wait(). Returns NULL if the promise is not a file request. */
FILE *fate_async_fopen(fate_async *a);

/* Available only to file promises. Waits until data has completely been committed. 
 * For read-only files or files that are still downloading, 
 * this is a no-op since there's nothing to do. */
void fate_async_commit(fate_async *a);

/* Applies to all promises.
 * If the file is still downloading, aborts the request.
 * Else, performs an implicit commit() and releases resources. */
void fate_async_close(fate_async *a);


#endif /* FATE_ASYNC_H */
