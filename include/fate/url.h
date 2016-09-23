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

#ifndef FE_URL_H
#define FE_URL_H

/*! \file fate/url.h
 *  \brief URL utilities.
 *  \defgroup url URL utilities.
 *
 * @{
 */

#include <stdint.h>
#include <stdbool.h>
#include <fate/decl.h>

/*! \brief TODO 
 *
 * The strings are all UTF-8. They're encoded according
 * by #fe_urlparts_to_url(); */
typedef struct {
    char *scheme; /* http | https | file */
    char *user;
    char *password;
    char *host; /* www.example.com */
    uint16_t port;
    char *path; /* /foo/index.html */
    char *query; /* ?foo=2&bar=3 */
    char *fragment; /* #anchor */
} fe_urlparts;

/*! \brief You should then free the returned URL with #fe_mem_heapfree(). */
FE_DECL_NIY char *fe_urlparts_to_url(const fe_urlparts *parts);
/*! \brief You should then free the URL parts' members with #fe_urlparts_deinit(). */
FE_DECL_NIY bool fe_urlparts_from_url(fe_urlparts *parts, const char *url);
/*! \brief TODO */
FE_DECL_NIY void fe_urlparts_deinit(fe_urlparts *parts);
/*! \brief You should then free the returned string with #fe_mem_heapfree().  */
FE_DECL_NIY char *fe_percent_encode(const char *utf8);
/*! \brief You should then free the returned string with #fe_mem_heapfree().  */
FE_DECL_NIY char *fe_punycode_encode(const char *utf8);

/*! @} */

#endif /* FE_URL_H */
