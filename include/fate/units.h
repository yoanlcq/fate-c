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

/*! \file fate/units.h
 *  \brief Typedefs for units in space and time.
 *  \defgroup units Units : Typedefs for space units and time units.
 *
 * This file is more like a configuration file. Depending on the game,
 * we might want to use bigger or shorter types for representing space units
 * and time units.
 *
 * There's no direct consequence on the size of network packets, since
 * most of the time, only deltas are sent, and those deltas just take the
 * minimum amount of bits required.\n
 * However there may be performance issues depending on the target platform or
 * hardware (e.g Emscripten doesn't like 64-bit integers), aside from the
 * obvious impact on memory (cache size also comes to mind).\n
 * This is not to be taken lightly and this is why the types must be 
 * tuned for the target game, if not for the target platform/hardware.
 *
 * Whatever you do, always \c typedef units to integers.\n
 * It's the renderer's job to map units to its specific coordinate system.\n
 * Units themselves are pure game logic shared by players, and as such 
 * they must not be subject to client-specific precision issues.
 *
 * The math module has vector types which use the units specified in this
 * file.
 * See #spacevec3 for instance.
 *
 * @{
 */

#ifndef FE_UNITS_H
#define FE_UNITS_H

#include <stdint.h>

/*! \brief Type representing a space unit. Must be a signed integer type. */
typedef  int64_t fe_space_unit;

/*! \brief Type representing a time unit. Must be an unsigned integer type. 
 *
 * Don't subtract two unsigned types. Use #fe_uint_diff instead.
 * \endcode
 */
typedef uint64_t fe_time_unit;

/*! \brief Type representing an opacity unit. Must be an unsigned integer type. 
 *
 * Don't subtract two unsigned types. Use #fe_uint_diff instead.
 * \endcode
 */
typedef uint8_t fe_alpha_unit;

/*! \brief Compute properly the difference between two unsigned 
 *         integers of any size.
 *
 * Stores the absolute value of \p a - \p b at the address pointed to by
 * \p diff, and returns a flag which indicates if \p diff must be considered
 * as negative.\n
 * When the result is zero, the said flag is guaranteed not to be raised.
 *
 * There's no typo in the function's name. It's indeed #fe_uint_diff
 * (for unsigned integer) instead of \c fe_unit_diff.
 *
 * \param diff Address of an unsigned integer where to store the absolute value 
 *             of the difference between \p a and \p b.
 * \param a Unsigned left-hand operand.
 * \param b Unsigned right-hand operand.
 * \return Is the difference negative ?
 */
#define fe_uint_diff(diff,a,b) (a>=b ? (*diff=a-b, 0) : (*diff=b-a, 1))

#endif /* FE_UNITS_H */

/* @} */
