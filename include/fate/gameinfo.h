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

/*! \file fate/gameinfo.h
 *  \brief List of per-game administrative constants
 *  \defgroup gameinfo List of per-game administrative constants
 *
 * Imagine that the \c FE_ prefix is replaced by your game's.
 *
 * The constants are not placed into a struct, for the following reasons :
 * -> That struct would be global;
 * -> Binary compatibility => ouch;
 * -> With defines, only the data we actually use ends up in the executable;
 * -> With defines, it's easier to concatenate strings at compile-time.
 *
 * @{
 */

#ifndef FE_GAMEINFO_H
#define FE_GAMEINFO_H

#define FE_GAMEINFO_URL              "http://cube.author.org/"
#define FE_GAMEINFO_COMPANY_NAME     "Author Studios inc."
#define FE_GAMEINFO_DESCRIPTION      "A Cube spinning around."
#define FE_GAMEINFO_LEGAL_COPYRIGHT  "Author Foo"
#define FE_GAMEINFO_LEGAL_TRADEMARKS "Authorware"
#define FE_GAMEINFO_NAME             "Cube Demo"
#define FE_GAMEINFO_NAME_IDENT       "cube_demo"
#define FE_GAMEINFO_EXECUTABLE_NAME  "cube_demo"
#define FE_GAMEINFO_VERSION_STRING   "1.0"
#define FE_GAMEINFO_DOMAIN           "org.author.cube"
#define FE_GAMEINFO_ANDROID_ACTIVITY "CubeActivity"
#define FE_GAMEINFO_TARGET_OS        "Linux"
#define FE_GAMEINFO_TARGET_ARCH      "x86_64"
#define FE_GAMEINFO_TARGET_BITS      64
#define FE_GAMEINFO_COMPILER         "gcc"

/*! @} */

#endif /* FE_GAMEINFO_H */
