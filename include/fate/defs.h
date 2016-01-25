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

/*! \file fate/defs.h
 *  \brief Globally available macro definitions.
 *
 *  TODO
 */

#ifndef FATE_DEFS_H
#define FATE_DEFS_H

#define FATE_VERSION_MAJOR 0
#define FATE_VERSION_MINOR 1
#define FATE_VERSION_PATCH 0
#define FATE_VERSION_CODENAME "Rainbow Dice"

#define XSTRINGIFY(X) STRINGIFY(X)
#define STRINGIFY(X) #X

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

#include <fate/defs_compiler.h>
#include <fate/defs_platform.h>

#include <fate/log.h>

#endif /* FATE_DEFS_H */
