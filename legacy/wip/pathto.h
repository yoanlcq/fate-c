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
