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

/*! \file fate/io.h
 *  \brief The I/O module.
 *  \defgroup io I/O : Asynchronous memory stream.
 *
 * We often DON'T read/write files in a sparse manner.
 * Our files are pre-cooked, and the whole data needs to be in memory at once.
 * This is pretty much enforced by the fact that Emscripten's wget() reads whole files.
 * Streams are handled by a dedicated system in FATE, much like other component systems.
 *
 * So, an fe_io object is simply a single buffer representing a read-write heap memory stream.
 *
 * @{
 */


#ifndef FE_IO_H
#define FE_IO_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <fate/promise.h>

/*! \brief TODO
 * 
 * All members are private as long as received_size<total_size. */
typedef struct {
    void  *data;
    size_t data_size, received_size, allocated_size;
} fe_io;

/*! \brief TODO*/
enum fe_io_file_flags {
    FE_IOFF_ANDROID_INTERNAL_STORAGE      = 1<<0, 
    FE_IOFF_ANDROID_EXTERNAL_STORAGE      = 1<<1,
    FE_IOFF_WINRT_PATH_LOCAL_FOLDER       = 1<<2,
    FE_IOFF_WINRT_PATH_INSTALLED_LOCATION = 1<<3,
    FE_IOFF_SDL2_GETBASEPATH              = 1<<4,
    FE_IOFF_SDL2_GETPREFPATH              = 1<<5
};
/*! \brief TODO*/
typedef enum fe_io_file_flags fe_io_file_flags;

/*! \brief TODO  */
FE_NIY void fe_io___zallocv(fe_io *s, size_t cnt);
/*! \brief TODO  */
FE_NIY void fe_io___deallocv(fe_io *s, size_t cnt);
/*! \brief Reads a full file into memory using mmap() or MapViewOfFile().
 * Falls back to #fe_io_open_file() if required. 
 *
 *  */
FE_NIY void fe_io_open_mmap(fe_io io, const char *filename, fe_io_file_flags flags);
/*! \brief Reads a full file into memory, the closes it. On Emscripten, it uses MEMFS.
 * For the game dev, its use is discouraged in favor of fe_io_open_res(). 
 *  */
FE_NIY fe_promise fe_io_load_file(fe_io io, const char *filename, fe_io_file_flags flags);
/*! \brief Downloads a file, no matter the platform, opening then closing the connection. 
 *  */
FE_NIY fe_promise fe_io_load_wget(fe_io io, const char *url);
/*! \brief Either opens a file or downloads it depending if we are on Emscripten 
 * or not. 
 *  */
FE_NIY fe_promise fe_io_load_res(fe_io io, const char *url, const char *file, fe_io_file_flags flags);
/*! \brief Either opens a file or an IndexedDB entry depending if we are on Emscripten 
 * or not. 
 *  */
FE_NIY fe_promise fe_io_load_userdata(fe_io io, const char *file, fe_io_file_flags flags);
/*! \brief This is just because it's handy for logging info. If you know you're reading
 * a single file, you could put that file's name in it.
 * The string is duplicated on purpose, so there's no need to worry about its lifetime. 
 * It is free()'d at each next call to fe_io_name(). 
 *  */
FE_NIY void fe_io_name(fe_io io, const char *name);
/*! \brief TODO  */
FE_NIY const char *fe_io_get_name(fe_io io);

/* The following calls wait for the data to be ready : */
/*! \brief TODO */
FE_NIY void *fe_io_getdata(fe_io io);
/*! \brief TODO */
FE_NIY size_t fe_io_get_bytes_received(fe_io io);
/*! \brief TODO */
FE_NIY size_t fe_io_get_bytes_total(fe_io io);
/*! \brief TODO */
#define fe_io_getsize(s) fe_io_get_bytes_total(s)
/*! \brief TODO */
FE_NIY bool fe_io_ok(fe_io io);

/*! \brief TODO  */
typedef int fe_io_status;
/*! \brief TODO  */
FE_NIY fe_io_status fe_io_get_status(fe_io io);
/*! \brief TODO  */
FE_NIY const char *fe_io_status_str(fe_io_status status);

/*! \brief TODO  Actually just frees the memory. */
FE_NIY void fe_io_close(fe_io io);
/*! \brief TODO  Changes the 'allocated_size' member. */
FE_NIY void fe_io_resize(fe_io io, size_t new_size);
/* No read(), gets() or scanf() functions are provided, since you can get a pointer on the data. */
/*! \brief TODO */
FE_NIY void fe_io_write(fe_io io, const void *data, size_t size);
/*! \brief TODO */
FE_NIY void fe_io_putc(fe_io io, char c);
/*! \brief TODO */
FE_NIY void fe_io_puts(fe_io io, const char *str);
/*! \brief TODO */
FE_NIY void fe_io_printf(fe_io io, const char *fmt, ...);
/*! \brief Completely overwrites a file. Very few use cases, since it is 
 * neither userdata nor res. 
 *   */
FE_NIY fe_promise fe_io_sync_file(fe_io io, const char *filename, fe_io_file_flags flags);
/*! \brief Completely overwrites a userdata file.  
 *  */
FE_NIY fe_promise fe_io_sync_userdata(fe_io io, const char *filename, fe_io_file_flags flags);

/*! @} */
#endif /* FE_IO_H */
