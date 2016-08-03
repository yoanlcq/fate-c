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

/*! \file fate/timestamp.h
 *  \brief Small timestamp helper.
 *  \defgroup timestamp Small timestamp helper.
 *
 * @{
 */

#ifndef FE_TIMESTAMP_H
#define FE_TIMESTAMP_H

#include <stdint.h>
#include <time.h>
typedef int64_t fe_timestamp;
/* This function expects its parameters to be formatted EXACTLY like
 * __DATE__ and __TIME__ macros. */
fe_timestamp fe_timestamp_of_cpp_date_time(const char *date, const char *time);
#define fe_timestamp_here() fe_timestamp_of_cpp_date_time(__DATE__, __TIME__)
#define fe_timestamp_get_now() ((fe_timestamp)time(NULL))

/*! @} */

#endif /* FE_TIMESTAMP_H */
