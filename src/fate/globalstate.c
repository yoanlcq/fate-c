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
#include <fate/sys.h>
#include <fate/log.h>
#include <fate/globalstate.h>
#include <fate/i18n.h>

fate_globalstate fate_gs_;
fate_globalstate *fate_gs = &fate_gs_;

/* See https://kripken.github.io/emscripten-site/docs/api_reference/emscripten.h.html#emscripten-asynchronous-indexeddb-api */

void fate_globalstate_init(fate_globalstate *gs) {
    fate_log_setup();
    fate_i18n_setup();
    char *game_path = fate_sys_getgamepath();
    if(!game_path)
        fate_fatal("globalstate", 
                   _("Couldn't find the game's path.\nSorry.\n"));
    fate_sys_set_current_directory(game_path);
    free(game_path);

    fate_sys_crash_handler_setup();
}
void fate_globalstate_deinit(fate_globalstate *gs) {
}
