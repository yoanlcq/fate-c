#ifndef FE_IO_H
#define FE_IO_H

typedef struct fe_loader_callback_data fe_loader_callback_data;
typedef void (*fe_loader_callback)(const fe_loader_callback_data *data);
typedef uint32_t fe_loader;
bool fe_loader_wait(fe_loader l);
bool fe_loader_is_ready(fe_loader l);
void fe_loader_set_callback(fe_loader l, fe_loader_callback func, void *userdata);


/* We often DON'T read/write files in a sparse manner.
 * Our files are pre-cooked, and the whole data needs to be in memory at once.
 * This is pretty much enforced by the fact that Emscripten's wget() reads whole files.
 * Streams are handled by a dedicated system in FATE, much like other component systems.
 *
 * So, an fe_io object is simply a single buffer representing a read-write heap memory stream.
 */

/* All members are private as long as received_size<total_size. */
typedef struct {
    void  *data;
    size_t data_size, received_size, allocated_size;
} fe_io;

enum fe_io_file_flags {
    FE_IOFF_ANDROID_INTERNAL_STORAGE      = 1<<0, 
    FE_IOFF_ANDROID_EXTERNAL_STORAGE      = 1<<1,
    FE_IOFF_WINRT_PATH_LOCAL_FOLDER       = 1<<2,
    FE_IOFF_WINRT_PATH_INSTALLED_LOCATION = 1<<3,
    FE_IOFF_SDL2_GETBASEPATH              = 1<<4,
    FE_IOFF_SDL2_GETPREFPATH              = 1<<5
};
typedef enum fe_io_file_flags fe_io_file_flags;

void fe_io___zallocv(fe_io *s, size_t cnt);
void fe_io___deallocv(fe_io *s, size_t cnt);
/* Reads a full file into memory using mmap() or MapViewOfFile().
 * Falls back to #fe_io_open_file() if required. */
void fe_io_open_mmap(fe_io io, const char *filename, fe_io_file_flags flags);
/* Reads a full file into memory, the closes it. On Emscripten, it uses MEMFS.
 * For the game dev, its use is discouraged in favor of fe_io_open_res(). */
fe_loader fe_io_load_file(fe_io io, const char *filename, fe_io_file_flags flags);
/* Downloads a file, no matter the platform, opening then closing the connection. */
fe_loader fe_io_load_wget(fe_io io, const char *url);
/* Either opens a file or downloads it depending if we are on Emscripten 
 * or not. */
fe_loader fe_io_load_res(fe_io io, const char *url, const char *file, fe_io_file_flags flags);
/* Either opens a file or an IndexedDB entry depending if we are on Emscripten 
 * or not. */
fe_loader fe_io_load_userdata(fe_io io, const char *file, fe_io_file_flags flags);
/* This is just because it's handy for logging info. If you know you're reading
 * a single file, you could put that file's name in it.
 * The string is duplicated on purpose, so there's no need to worry about its lifetime. 
 * It is free()'d at each next call to fe_io_name(). */
void fe_io_name(fe_io io, const char *name);
const char *fe_io_get_name(fe_io io);

/* The following calls wait for the data to be ready : */
void *fe_io_getdata(fe_io io);
size_t fe_io_get_bytes_received(fe_io io);
size_t fe_io_get_bytes_total(fe_io io);
#define fe_io_getsize(s) fe_io_get_bytes_total(s)
bool fe_io_ok(fe_io io);

typedef int fe_io_status;
fe_io_status fe_io_get_status(fe_io io);
const char *fe_io_status_str(fe_io_status status);

/* Actually just frees the memory. */
void fe_io_close(fe_io io);
/* Changes the 'allocated_size' member. */
void fe_io_resize(fe_io io, size_t new_size);
/* No read(), gets() or scanf() functions are provided, since you can get a pointer on the data. */
void fe_io_write(fe_io io, const void *data, size_t size);
void fe_io_putc(fe_io io, char c);
void fe_io_puts(fe_io io, const char *str);
void fe_io_printf(fe_io io, const char *fmt, ...);
/* Completely overwrites a file. Very few use cases, since it is 
 * neither userdata nor res. */
fe_loader fe_io_sync_file(fe_io io, const char *filename, fe_io_file_flags flags);
/* Completely overwrites a userdata file. */
fe_loader fe_io_sync_userdata(fe_io io, const char *filename, fe_io_file_flags flags);

fe_loader fe_userdata_exists(const char *filename);
fe_loader fe_userdata_delete(const char *filename);
bool fe_file_exists(const char *filename);
void fe_file_delete(const char *filename);
uint64_t fe_file_get_last_write_time(const char *path);
void fe_file_move(const char *dst, const char *src);
void fe_file_copy(const char *dst, const char *src);
typedef int fe_filewatch;
fe_filewatch fe_file_watch(const char *filename);
void fe_file_unwatch(fe_filewatch watch);
void fe_sys_setcwd(const char *wd);
const char* fe_sys_getcwd();
void fe_sys_pushd(const char *wd);
void fe_sys_popd(void);

#endif /* FE_IO_H */
