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

/*! \file fate/dpyres.h
 *  \brief Display Resolutions.
 *
 * A list of (theoretically) all the possible display resolutions, 
 * ordered by width, then height.
 * Taken from https://upload.wikimedia.org/wikipedia/commons/0/0c/Vector_Video_Standards8.svg 
 * and Steam's 2015 Hardware Survey.
 * Last updated on July 2015.
 */

#ifndef FATE_DPYRES_H
#define FATE_DPYRES_H

#include <stdint.h>

#define FATE_DPYRES_TRUST_STEAM_2015_SURVEY

#ifdef FATE_DPYRES_TRUST_STEAM_2015_SURVEY
#define FATE_DPYRES_LEN 13
#else 
#define FATE_DPYRES_LEN 50
#endif

extern const uint16_t FATE_DPYRES[FATE_DPYRES_LEN][2];
const uint16_t *fate_dpyres_prev_in_list(uint16_t width, uint16_t height);
const uint16_t *fate_dpyres_next_in_list(uint16_t width, uint16_t height);
const uint16_t *fate_dpyres_prev(uint16_t width, uint16_t height);
const uint16_t *fate_dpyres_next(uint16_t width, uint16_t height);


#endif /* FATE_DPYRES_H */
