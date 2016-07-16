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

/*! \file fate/mem.h
 *  \brief Debug malloc() wrappers, heap inspection, and more.
 *  \defgroup mem Mem : Debug malloc() wrappers, heap inspection, and more.
 *
 * <b>Compile-time switches :</b>
 * <table>
 * <tr><td>FE_MEM_DEBUG</td><td>If not defined, heap allocator 
 *  macros expand to regular stdlib calls, and debug/profiling-related
 *  functionalities are compiled out, simply replaced by assignment 
 *  operators when it makes sense.</td></tr>
 * </table>
 * 
 * @{
 */


#ifndef FE_MEM_H
#define FE_MEM_H

/*! \brief TODO */
FE_NIY void fe_mem_setup(void);

#include <fate/mem/heap.h>
#include <fate/mem/stack.h>

/*! @} */

#endif /* FE_MEM_H */
