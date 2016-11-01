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

#ifndef FE_SYSERR_H
#define FE_SYSERR_H

/*! \file fate/syserr.h
 *  \brief OS-specific error code utilities
 *  \defgroup syserr OS-specific error code utilities
 *
 * @{
 */

#ifdef FE_TARGET_WINDOWS
typedef DWORD fe_syserr;
#define fe_syserr_set_last_error(err)   SetLastError(err)
#define fe_syserr_get_last_error()      GetLastError()
#define fe_syserr_is_user_defined(code) (!!(code & (1<<29)))
#else
typedef int fe_syserr;
#define fe_syserr_set_last_error(err)   (errno = (err))
#define fe_syserr_get_last_error()      (errno)
#define fe_syserr_is_user_defined(code) (code < 0)
#endif

FE_DECL_PURE
static inline fe_syserr fe_syserr_user_defined(int code) {
    fe_dbg_assert(code>0);
#ifdef FE_TARGET_WINDOWS
    return (1<<29) | code;
#else
    return -code;
#endif
}

/*! \brief Returns a heap-allocated string describing the error.
 *         Free it with fe_mem_heapfree(). 
 *
 * Currently, the message is in english (US), but some OSes allow
 * it to be generated using the user's locale. An upcoming function
 * should be provided to make us of this.
 * Also, there's no way to make this function return a custom string for
 * user-defined error codes. Supporting macros or function will be provided
 * sometime later.
 */
char* fe_syserr_str(fe_syserr error);


/*! @} */

#endif /* FE_SYSERR_H */
