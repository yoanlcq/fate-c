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


#ifndef FATE_H
#define FATE_H

/*! \file fate.h
 *  \brief A single file for including all of F.A.T.E's interfaces.
 *
 * TODO
 */

#include <fate/defs.h>
#include <fate/al.h>
#include <fate/gl.h>
#ifdef FATE_WINDOWS
#include <fate/d3d.h>
#endif
#include <fate/file_to_string.h>
#include <fate/globalstate.h>
#include <fate/log.h>
#include <fate/sys.h>
#include <fate/dpyres.h>
#include <fate/tracer.h>
#include <fate/vfs.h>

#endif /* FATE_H */