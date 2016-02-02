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

/*! \file fate/defs_compiler.h
 *  \brief Globally available compiler-specific macro definitions.
 *
 *  TODO
 */

#ifndef FATE_DEFS_COMPILER
#define FATE_DEFS_COMPILER

#if __STDC_VERSION__ < 199901L
#define __func__ ""
#endif

/* Obviously, see the GCC docs. */
#ifdef __GNUC__

#define FATE_DEPRECATED(msg) __attribute__((deprecated(msg)))
#define FATE_PRINTF_DECL __attribute__((format(printf, 1, 2)))
#define FATE_NONNULL_PARAMS(arg_index,...) __attribute__((nonnull(arg_index, __VA_ARGS__)))
#define FATE_SENTINEL(pos) __attribute__((sentinel(pos)))
#define FATE_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#define FATE_PREFETCH(addr,rw,locality) __builtin_prefetch(addr,rw,locality)
#define FATE_PACKED __attribute__((packed))
#define FATE_EXPECTED(cond) __builtin_expect(!(cond),0)

#else

#define FATE_DEPRECATED(msg)
#define FATE_PRINTF_DECL 
#define FATE_NONNULL_PARAMS(arg_index,...)
#define FATE_SENTINEL(pos)
#define FATE_WARN_UNUSED_RESULT
#define FATE_PREFETCH(addr,rw,locality)
#define FATE_PACKED
#define FATE_EXPECTED(cond) cond

#endif

#ifdef _MSC_VER
#include <sal.h>
#undef  FATE_WARN_UNUSED_RESULT
#define FATE_WARN_UNUSED_RESULT _Check_return_
#endif

#endif /* FATE_DEFS_COMPILER */
