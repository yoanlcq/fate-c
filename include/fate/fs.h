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

/*! \file fate/fs.h
 *  \brief File system.
 *  \defgroup fs File system.
 *
 * @{
 */

#ifndef FE_FS_H
#define FE_FS_H

#include <fate/defs.h>
#include <fate/iov.h>
#include <stdbool.h>

/*! \brief TODO */
char *fe_fs_get_executable_dir(void);

/*! \brief TODO */
bool  fe_fs_setcwd(const fe_iov_locator *params);
/*! \brief TODO */
FE_DECL_NIY char* fe_fs_getcwd(void);
/*! \brief TODO */
uint64_t fe_fs_file_get_wtime(const fe_iov_locator *params);

/*! \brief TODO */
FE_DECL_NIY fe_iov_status  fe_fs_file_exists(const fe_iov_locator *params);
/*! \brief TODO */
FE_DECL_NIY fe_iov_status  fe_fs_file_delete(const fe_iov_locator *params);
/*! \brief TODO */
FE_DECL_NIY fe_iov_status  fe_fs_persistent_exists(const fe_iov_locator *params);
/*! \brief TODO */
FE_DECL_NIY fe_iov_promise fe_fs_persistent_exists_async(const fe_iov_locator *params);
/*! \brief TODO */
fe_iov_status  fe_fs_persistent_delete(const fe_iov_locator *params);
/*! \brief TODO */
FE_DECL_NIY fe_iov_promise fe_fs_persistent_delete_async(const fe_iov_locator *params);


/*! @} */

#endif /* FE_FS_H */
