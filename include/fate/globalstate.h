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

/*! \file fate/globalstate.h
 *  \brief Definition for a Global State object.
 *  \defgroup globalstate Global state
 *
 * TODO
 *
 * @{
 */

#ifndef FE_GLOBALSTATE_H
#define FE_GLOBALSTATE_H

#include <fate/defs.h>

/*! \brief TODO */
struct fe_globalstate {
    int foo;
    /* fe_vfs vfs; */
};
/*! \brief TODO */
typedef struct fe_globalstate fe_globalstate;

/*! \brief TODO */
extern fe_globalstate *fe_gs;

/*! \brief TODO */
void fe_globalstate_init(fe_globalstate *gs);
/*! \brief TODO */
void fe_globalstate_deinit(fe_globalstate *gs);

/*! @} */
#endif /* FE_GLOBALSTATE_H */
