#ifndef FATE_asyncfile_H
#define FATE_asyncfile_H

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

typedef enum fate_asyncfile_origin fate_asyncfile_origin;

struct fate_asyncfile {
    FILE *file;
    float progress;
};
typedef struct fate_asyncfile fate_asyncfile;

/* Starts downloading the file. */
fate_async_clientfile(fate_asyncfile *f, const char *path, const char *mode);
fate_async_serverfile(fate_asyncfile *f, const char *path, const char *mode);
fate_async_task(fate_asyncfile *f, void (*task)(void *arg));
fate_async_request(fate_asyncfile *f, fate_async_namespace ns, const char *path, const char *mode);
/* Yield some time for downloading files.
 * The first parameter is only a hint and might not be used. */
void fate_async_yield(fate_asyncfile *f, unsigned ms);
/* returns less than 0.0f in case of error. */
float fate_async_getprogress(fate_asyncfile *f);
/* Wait for a file to finish downloading. Supposedly better than 
 * active waiting with yield() and getprogress(). */
void fate_async_wait(fate_asyncfile *f);
/* Performs and implicit wait(). */
FILE *fate_async_fopen(fate_asyncfile *f);
/* Waits until data has completely been committed. */
void fate_async_commit(fate_asyncfile *f);
/* If the file is still downloading, aborts the request.
 * Else, performs and implicit commit() and releases resources. */
void fate_async_close(fate_asyncfile *f);

#endif /* FATE_asyncfile_H */
