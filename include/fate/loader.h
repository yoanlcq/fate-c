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

/*! \file fate/loader.h
 *  \brief Loaders.
 *  \defgroup loader Loader
 *
 * @{
 */

#ifndef FE_LOADER_H
#define FE_LOADER_H

#include <stdbool.h>
#include <stdint.h>

/*! \brief TODO*/
typedef struct fe_loader_callback_data fe_loader_callback_data;
/*! \brief TODO*/
typedef void (*fe_loader_callback)(const fe_loader_callback_data *data);
/*! \brief TODO*/
typedef uint32_t fe_loader;
/*! \brief TODO*/
bool fe_loader_wait(fe_loader l);
/*! \brief TODO*/
bool fe_loader_is_ready(fe_loader l);
/*! \brief TODO*/
void fe_loader_set_callback(fe_loader l, fe_loader_callback func, void *userdata);


/*! @} */
#endif /* FE_LOADER_H */
