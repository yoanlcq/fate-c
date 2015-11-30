#ifndef PATHTO_H
#define PATHTO_H

#include <stdint.h>

/* C++ warning */
/* #pragma GCC diagnostic ignored "-Wwrite-strings" */

#if defined(__WIN32)
#include <windows.h>
#define exists(_p_) (GetFileAttributes(_p_) != INVALID_FILE_ATTRIBUTES)
#else 
#include <unistd.h>
#define exists(_p_) (!access(_p_, F_OK))
#endif

/* 
 * pathto() returns its parameter, except that if path starts with '@', 
 * the '@' is replaced by the absolute full path of the portable app's
 * directory (aka the one that containes res/ and data/).
 * In that case, the returned string is dynamically allocated, BUT IN NO CASE 
 * should the caller attempt to free() it.
 * To allow nested calls of pathto(), a circular buffer is maintained to 
 * free the oldest (and supposedly unused) string previously returned.
 * The size of that circular buffer can be defined below.
 */
#define PATHTO_CIRCULAR_BUFFER_SIZE 8
char *pathto(char *path);

#endif /* PATHTO_H */
