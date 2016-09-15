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

#ifndef FE_DECL_H
#define FE_DECL_H

/*! \file fate/decl.h
 *  \brief Declaration attributes wrappers.
 *  \defgroup decl Declaration attribute wrappers.
 *
 * All macros in this header file expand to nothing whenever the
 * compiler cannot implement them.
 * With GCC and Clang, they are often defined to \c __attribute__((foo)).
 * With MSVC, they may be defined to \c __declspec(foo).
 *
 * @{
 */


#include <fate/defs.h>

#if __DOXYGEN__ || defined(__GNUC__)

/*! \brief Marks a function or object as deprecated, displaying a custom 
 *         message.
 *
 * GCC, Clang, MSVC. */
#define FE_DECL_DEPRECATED(msg) __attribute__((deprecated(msg)))

/*! \brief Marks a function as malloc-like.
 *
 * GCC, Clang.
 */
#define FE_DECL_MALLOC __attribute__((malloc))

/*! \brief Marks a function as printf-like.
 *
 * This gives to the compiler the ability to check formats given to the marked
 * functions.
 *
 * GCC, Clang.
 *
 * \param fmt_index The index of the "format" parameter, starting from 1.
 * \param args_index The index of the "..." parameter, starting from 1.
 */
/* We say __printf__ instead of printf because libintl.h may redefine printf as 
 * _libintl_printf. */
#define FE_DECL_PRINTF(fmt_index, args_index) \
            __attribute__((format(__printf__, fmt_index, args_index)))

/*! \brief Instructs the compiler that some arguments to a function cannot be 
 *         NULL. 
 *
 * GCC, Clang.
 *
 * This macro takes a variable arguments list of indices which should indicate
 * which parameters are to be non-NULL.
 * Indices start from 1.
 */
#define FE_DECL_NONNULL_PARAMS(arg_index,...) \
            __attribute__((nonnull(arg_index, __VA_ARGS__)))

/*! \brief Instructs the compiler that the variable arguments given to a 
 *         function must be ended by NULL. 
 *
 * GCC, Clang.
 *
 * \param pos Where must the sentinel be located, counting backwards from the 
 *            end of the arguments list ? */
#define FE_DECL_SENTINEL(pos) __attribute__((sentinel(pos)))

/*! \brief Instructs the compiler that the function's result should not be
 *         ignored. 
 *
 * GCC, Clang, MSVC.
 */
#define FE_DECL_WARN_UNUSED_RESULT __attribute__((warn_unused_result))

/*! \brief Asks the compiler to reduce the size taken by instances of the
 *         marked struct as much as possible. 
 *
 * GCC, Clang.
 */
#define FE_DECL_PACKED_STRUCT __attribute__((packed))

/*! \brief Expands to __attribute__((warning(...))) if supported.
 *
 * GCC.
 */
#define FE_DECL_WARN_IF_USED(str) __attribute__((warning(str)))
#ifdef __clang__
#undef FE_DECL_WARN_IF_USED
#define FE_DECL_WARN_IF_USED(str)
#endif

/*! \brief Marks a function as "export" when building a DLL. 
 *
 * GCC, Clang, MSVC.
 */
#define FE_DECL_DLLEXPORT __attribute__((dllexport))

/*! \brief Marks a function as "pure", that is, its return value is only
 *         computed from its parameters and has no side-effects. 
 *
 * GCC, Clang.
 */
#define FE_DECL_PURE __attribute__((pure))

/*! \brief Marks a function as "hot", that is, used often.
 *
 * GCC, Clang.
 */
#define FE_DECL_USED_OFTEN __attribute__((hot))

/*! \brief Marks a function as "cold", that is, used rarely.
 *
 * GCC, Clang.
 */
#define FE_DECL_USED_RARELY __attribute__((cold))

/*! \brief Compile-time specifier for a declaration (not a typedef) 
 *         requiring a specific alignment.
 *
 * GCC, Clang, MSVC.
 *
 * This matters when using some low-level instructions such as 
 * _mm_stream_si128().
 */
#define FE_DECL_ALIGN(n) __attribute__((align(n)))


#elif defined(_MSC_VER) /* ifdef __GNUC__ */


#include <sal.h>
#define FE_DECL_WARN_UNUSED_RESULT _Check_return_
#define FE_DECL_DLLEXPORT __declspec(dllexport)
#define FE_DECL_ALIGN(n) __declspec(align(n))
#define FE_DECL_DEPRECATED(str) __declspec(deprecated(str))
#define FE_DECL_WARN_IF_USED(str) __declspec(deprecated(str))

#else

#define FE_DECL_DEPRECATED(msg) 
#define FE_DECL_MALLOC
#define FE_DECL_PRINTF(fmt_index, args_index) 
#define FE_DECL_NONNULL_PARAMS(arg_index,...) 
#define FE_DECL_SENTINEL(pos) 
#define FE_DECL_WARN_UNUSED_RESULT 
#define FE_DECL_PACKED_STRUCT
#define FE_DECL_WARN_IF_USED(str)
#define FE_DECL_DLLEXPORT
#define FE_DECL_PURE
#define FE_DECL_USED_OFTEN
#define FE_DECL_USED_RARELY
/* Fail on this because this is very important. */
#error "FE_DECL_ALIGN() can't be defined because your compiler doesn't support alignment specifiers."

#endif /* ifdef __GNUC_ */

/*! \brief Marks a function as "NOT IMPLEMENTED YET"
 *
 * Use this for functions that are at most 50% finished.
 * This macro expands to nothing if the compiler is not GCC.
 */
#define FE_DECL_NIY FE_DECL_WARN_IF_USED("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~NOT IMPLEMENTED YET~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")

/*! \brief Marks a function as "WORK IN PROGRESS"
 *
 * Use this for functions that are at least 50% finished.
 * This macro expands to nothing if the compiler is not GCC.
 */
#define FE_DECL_WIP FE_DECL_WARN_IF_USED("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~WORK IN PROGRESS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")



/*! @} */

#endif /* FE_DECL_H */
