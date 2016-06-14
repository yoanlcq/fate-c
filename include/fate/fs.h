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
 *  \brief Filesystem operations.
 *  \defgroup fs Filesystem operations.
 *
 * @{
 */



#ifndef FE_FS_H
#define FE_FS_H

#include <stdbool.h>
#include <fate/defs.h>
#include <fate/promise.h>

/*! \brief TODO */
FE_NIY fe_promise fe_userdata_exists(const char *filename);
/*! \brief TODO */
FE_NIY fe_promise fe_userdata_delete(const char *filename);
/*! \brief TODO */
bool fe_fs_exists(const char *filename);
/*! \brief TODO */
FE_NIY void fe_fs_delete(const char *filename);
/*! \brief TODO */
uint64_t fe_fs_get_last_write_time(const char *path);
/*! \brief TODO */
FE_NIY void fe_fs_move(const char *dst, const char *src);
/*! \brief TODO */
FE_NIY void fe_fs_copy(const char *dst, const char *src);
/*! \brief TODO */
typedef int fe_filewatch;
/*! \brief TODO */
FE_NIY fe_filewatch fe_fs_watch(const char *filename);
/*! \brief TODO */
FE_NIY void fe_fs_unwatch(fe_filewatch watch);
/*! \brief TODO */
bool fe_fs_setcwd(const char *wd);
/*! \brief TODO */
FE_NIY const char* fe_fs_getcwd();
/*! \brief TODO */
FE_NIY void fe_fs_pushd(const char *wd);
/*! \brief TODO */
FE_NIY void fe_fs_popd(void);

/*! @} */
#endif /* FE_FS_H */
