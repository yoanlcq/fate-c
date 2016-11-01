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
 *  \brief Display Resolutions table.
 *  \defgroup dpyres Dpyres : Display Resolutions table
 *
 * A list of (theoretically) all the possible display resolutions, 
 * ordered by width, then height.<br>
 * Taken from <a href="https://upload.wikimedia.org/wikipedia/commons/0/0c/Vector_Video_Standards8.svg" target="_blank">This SVG file</a>
 * and Steam's 2015 Hardware Survey.<br>
 * Last updated on July 2015.
 *
 * <b>Compile-time switches :</b>
 * <table>
 * <tr>
 * <td>FE_DPYRES_TRUST_STEAM_2015_SURVEY</td>
 * <td>If defined, reduces the list's size to only the most common display
 *     resolutions, based on Steam's 2015 Hardware Survey.<br>
 *     #FE_DPYRES_LEN expands to 13 instead of 50.</td>
 * </tr>
 * </table>
 * 
 * @{
 */

#ifndef FE_DPYRES_H
#define FE_DPYRES_H

#include <stdint.h>

/*! \brief The #FE_DPYRES array's length. */
#ifdef FE_DPYRES_TRUST_STEAM_2015_SURVEY
#define FE_DPYRES_LEN 13
#else 
#define FE_DPYRES_LEN 50
#endif

/*! \brief The display resolutions table.
 *
 * It is an array of width-height pairs, ordered by width, then height.
 */
extern const uint16_t FE_DPYRES[FE_DPYRES_LEN][2];
/*! \brief Gets the #FE_DPYRES entry that would come right before the given 
 *         display resolution. */
const uint16_t *fe_dpyres_prev_in_list(uint16_t width, uint16_t height);
/*! \brief Gets the #FE_DPYRES entry that would come right after the given 
 *         display resolution. */
const uint16_t *fe_dpyres_next_in_list(uint16_t width, uint16_t height);
/*! \brief Gets the highest #FE_DPYRES entry that is contained in the given 
 *         display resolution. */
const uint16_t *fe_dpyres_prev(uint16_t width, uint16_t height);
/*! \brief Gets the lowest #FE_DPYRES entry that contains the given 
 *         display resolution. */
const uint16_t *fe_dpyres_next(uint16_t width, uint16_t height);

/* @} */

#endif /* FE_DPYRES_H */
