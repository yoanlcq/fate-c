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

/*! \file fate/defs_platform.h
 *  \brief Platform-specific macro definitions.
 *
 *  TODO
 */

#ifndef FATE_DEFS_PLATFORM_H
#define FATE_DEFS_PLATFORM_H

#if defined(_WIN32) || defined(__WIN32__)
    #define FATE_WINDOWS
#elif defined(linux) || defined(__linux)
    #define FATE_LINUX
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) ||            defined(Macintosh)
    #define FATE_OSX
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
    #define FATE_FREEBSD
#elif defined(__EMSCRIPTEN__)
    #define FATE_EMSCRIPTEN
#else
    #error This platform is not supported by F.A.T.E.
#endif

#if FATE_WINDOWS
    #ifdef _WIN32_WINNT
        #if _WIN32_WINNT < 0x0600
            #undef _WIN32_WINNT
            #define _WIN32_WINNT 0x0600
        #endif
    #else
        #define _WIN32_WINNT 0x0600
    #endif
#endif

#endif /* FATE_DEFS_PLATFORM_H */
