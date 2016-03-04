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

/*! \file fate/asm.h
 *  \brief Architecture-specific functions (close to assembly).
 *  \defgroup asm Asm : Architecture-specific functions (close to assembly).
 *
 * TODO
 *
 * @{
 */


#ifndef FATE_ASM_H
#define FATE_ASM_H

#if __DOXYGEN__ || defined(__GNUC__)
/*! \brief Prefetch data, putting it the appropriate CPU cache level.
 *
 * It has been reported to provide interesting performance boosts <b>when 
 * used properly</b>. But as always, profile.
 *
 * The GCC manual recommends calling it early enough before the data 
 * is actually accessed.
 *
 * \param addr The data's address.
 * \param rw Compile-time boolean : "Will the data be written to ?"
 * \param locality Compile-time integer between 0 and 3 : "For how long
 *            should the data be kept in the cache ?"
 */
#define fate_asm_prefetch(addr,rw,locality) \
       __builtin_prefetch(addr,rw,locality)
/*! \brief Provides branch prediction information to the compiler. 
 *
 * This macro expands to nothing if the compiler is not GCC.
 */
#define fate_asm_likely(cond)   __builtin_expect(!(cond),0)
/*! \brief Provides branch prediction information to the compiler. 
 *
 * This macro expands to nothing if the compiler is not GCC.
 */
#define fate_asm_unlikely(cond) __builtin_expect((cond),0)

#else
#define fate_asm_prefetch(addr,rw,locality)
#define fate_asm_likely(cond)
#define fate_asm_unlikely(cond)
#endif

/*! @} */

#endif /* FATE_ASM_H */
