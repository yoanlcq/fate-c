#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <fate/defs.h>
#include <fate/io.h>

void fe_io___zallocv(fe_io *s, size_t cnt) {}
void fe_io___deallocv(fe_io *s, size_t cnt) {}
void fe_io_open_mmap(fe_io io, const char *filename, fe_io_file_flags flags) {}
fe_loader fe_io_load_file(fe_io io, const char *filename, fe_io_file_flags flags) {return 0;}
fe_loader fe_io_load_wget(fe_io io, const char *url) {return 0;}
fe_loader fe_io_load_res(fe_io io, const char *url, const char *file, fe_io_file_flags flags) {return 0;}
fe_loader fe_io_load_userdata(fe_io io, const char *file, fe_io_file_flags flags) {return 0;}
void fe_io_name(fe_io io, const char *name) {}
const char *fe_io_get_name(fe_io io) {return NULL;}

void *fe_io_getdata(fe_io io) {return NULL;}
size_t fe_io_get_bytes_received(fe_io io) {return 0;}
size_t fe_io_get_bytes_total(fe_io io) {return 0;}
bool fe_io_ok(fe_io io) {return false;}

fe_io_status fe_io_get_status(fe_io io) {return 0;}
const char *fe_io_status_str(fe_io_status status) {return "";}

/* Actually just frees the memory. */
void fe_io_close(fe_io io) {}
/* Changes the 'allocated_size' member. */
void fe_io_resize(fe_io io, size_t new_size) {}
/* No read(), gets() or scanf() functions are provided, since you can get a pointer on the data. */
void fe_io_write(fe_io io, const void *data, size_t size) {}
void fe_io_putc(fe_io io, char c) {}
void fe_io_puts(fe_io io, const char *str) {}
void fe_io_printf(fe_io io, const char *fmt, ...) {}
/* Completely overwrites a file. Very few use cases, since it is 
 * neither userdata nor res. */
fe_loader fe_io_sync_file(fe_io io, const char *filename, fe_io_file_flags flags) {return 0;}
/* Completely overwrites a userdata file. */
fe_loader fe_io_sync_userdata(fe_io io, const char *filename, fe_io_file_flags flags) {return 0;}

