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

/*! \file fate/utf8.h
 *  \brief The UTF-8 module.
 *  \defgroup utf8 The UTF-8 module.
 *
 * @{
 */

#ifndef FE_UTF8_H
#define FE_UTF8_H

#include <fate/defs.h>

#ifdef FE_TARGET_WINDOWS
#include <windows.h>
/*! \brief Free the string with #fe_mem_heapfree(). */
WCHAR *fe_utf8_to_win32unicode(const char *utf8);
/*! \brief Free the string with #fe_mem_heapfree(). */
char  *fe_utf8_from_win32unicode(const WCHAR *unicode);
#endif

/*! @} */

#endif /* FE_UTF8_H */
