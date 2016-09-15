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

/*! \file fate/globalstate.c
 *  \brief Global State.
 *
 * TODO
 */


#include <stdlib.h>
#include <stdio.h>
#include <fate/fate.h>

/* static const char *TAG = "fe_globalstate"; */
fe_globalstate fe_gs_;
fe_globalstate *fe_gs = &fe_gs_;

void fe_globalstate_init(fe_globalstate *gs) {
    fe_hw_setup();
    fe_mem_setup();
    fe_log_setup();
    fe_i18n_setup();
    fe_crash_setup();
    fe_iov_setup();
    fe_ipv6_setup();
    fe_hash_setup();
    fe_mt_setup();
#ifdef FE_TARGET_LINUX
    fe_linuxperf_setup();
#endif
#ifdef FE_TARGET_ENV_DESKTOP
    /* XXX Giving up on it for now. */
    /*
    char *game_dir = fe_fs_get_executable_dir();
    if(!game_dir)
        fe_fatal("fe_globalstate", 
            "Could not find the executable's directory.\n"
            "This is required for loading resources. Sorry.\n"
        );
    fe_iov_locator locator = {0};
    locator.file_name = game_dir;
    fe_logd(TAG, "Game dir: %s\n", game_dir);
    fe_fs_setcwd(&locator);
    fe_mem_heapfree(game_dir);
    */
#endif
}
void fe_globalstate_deinit(fe_globalstate *gs) {
    fe_mt_cleanup();
    fe_ipv6_cleanup();
    fe_iov_cleanup();
    fe_log_cleanup();
}
