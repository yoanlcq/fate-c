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

/*! \file fate/defs.h
 *  \brief Globally available macro definitions.
 *  \defgroup defs Defs : Globally available definitions.
 *
 * A lot of compile-time functionality is provided in this file.
 * Every module is encouraged to make best use of them.
 *
 * F.A.T.E's version follows <a href="http://semver.org/spec/v2.0.0.html" 
 * target="_blank">the Semantic Versioning 2.0.0 Specification</a>.
 *
 * @{
 */

#ifndef FATE_DEFS_H
#define FATE_DEFS_H

/*! \brief Stringify the expansion of the given expression. */
#define XSTRINGIFY(X) STRINGIFY(X)

/*! \brief Stringify the given expression. */
#define STRINGIFY(X) #X

/*! \brief The current F.A.T.E version's codename. 
 *
 * It serves no other purpose than being a fun way of identifying a version. 
 */
#define FATE_VERSION_CODENAME "Bookkeeper"
/*! \brief The current F.A.T.E version's prerelease string, conforming to 
 *         point 9 of
 *         <a href="http://semver.org/spec/v2.0.0.html" target="_blank">
 *         the Semantic Versioning 2.0.0 Specification</a>.
 *
 * Define it to something like "-alpha", "-beta",
 * or define it to the empty string.
 */
#define FATE_VERSION_PRERELEASE_STR
#define FATE_VERSION_MAJOR 0
#define FATE_VERSION_MINOR 1
#define FATE_VERSION_PATCH 0
/*! \brief The current F.A.T.E version's build metadata, conforming to 
 *         point 10 of
 *         <a href="http://semver.org/spec/v2.0.0.html" target="_blank">
 *         the Semantic Versioning 2.0.0 Specification</a>.
 *
 * This macro is supposed to be defined by the build process.
 * If not, it is defined to expand to nothing.
 * 
 * It must start with a '+', and may be, for instance, a date in this format :
 * \c "YYYYMMDDHHMMSS". Example : \c "+20130313144700".
 */
#ifndef FATE_VERSION_BUILD_METADATA
#define FATE_VERSION_BUILD_METADATA
#endif

/*! \brief The complete string identifying F.A.T.E's current version. */
#define FATE_VERSION \
    XSTRINGIFY(FATE_VERSION_MAJOR) "." XSTRINGIFY(FATE_VERSION_MINOR) "." \
    XSTRINGIFY(FATE_VERSION_PATCH) FATE_VERSION_PRERELEASE_STR \
    FATE_VERSION_BUILD_METADATA


/*! \brief Compiler/platform-specific "__declspec(dllexport)". */
#define FATE_EXPORT

#if __DOXYGEN__
/*! \brief Defined only if the target is Windows. */
#define FATE_TARGET_WINDOWS 1
/*! \brief Defined only if the target is Linux. */
#define FATE_TARGET_LINUX 1
/*! \brief Defined only if the target is Mac OS X. */
#define FATE_TARGET_OSX 1
/*! \brief Defined only if the target is FreeBSD. */
#define FATE_TARGET_FREEBSD 1
/*! \brief Defined only if the target is Android. */
#define FATE_TARGET_ANDROID 1
/*! \brief Defined only if the target is the iOS Simulator. */
#define FATE_TARGET_IOS_SIMULATOR 1
/*! \brief Defined only if the target is either iOS or the iOS Simulator. */
#define FATE_TARGET_IOS 1
/*! \brief Defined only if the target is Emscripten. */
#define FATE_TARGET_EMSCRIPTEN 1
#endif

#if defined(_WIN32) || defined(__WIN32__)
    #define FATE_TARGET_WINDOWS 1
#elif defined(linux) || defined(__linux)
    #define FATE_TARGET_LINUX 1
/*
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) \
      || defined(Macintosh)
    #define FATE_OSX
*/
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
        #define FATE_TARGET_IOS_SIMULATOR 1
        #define FATE_TARGET_IOS 1
    #elif TARGET_OS_IPHONE
        #define FATE_TARGET_IOS 1
    #elif TARGET_OS_MAC
        #define FATE_TARGET_OSX 1
    #else
        #error "Unknown Apple platform."
    #endif
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
    #define FATE_TARGET_FREEBSD 1
#elif defined(__EMSCRIPTEN__)
    #define FATE_TARGET_EMSCRIPTEN 1
#elif defined(__ANDROID__)
    #define FATE_TARGET_ANDROID 1
#else
    #error This platform is not supported by F.A.T.E.
#endif

#if __DOXYGEN__
#define _WIN32_WINNT 0x0600
#endif

#if FATE_TARGET_WINDOWS
    #ifdef _WIN32_WINNT
        #if _WIN32_WINNT < 0x0600
            #undef _WIN32_WINNT
            #define _WIN32_WINNT 0x0600
        #endif
    #else
        #define _WIN32_WINNT 0x0600
    #endif
#endif



/*
 *
 *
 * Version-related macros
 *
 *
 */





/*! \brief Compare two versions for equality. */
#define FATE_VERCMP_EQ(maj0, min0, pat0, maj1, min1, pat1) \
            ((maj0)==(maj1) && (min0)==(min1) && (pat0)==(pat1))

/*! \brief "Greater Than" version comparison. */
#define FATE_VERCMP_GT(maj0, min0, pat0, maj1, min1, pat1) \
           ((maj0)>(maj1) \
            || ((maj0)==(maj1) \
                && ((min0)>(min1) \
                    || ((min0)==(min1) \
                        && ((pat0)>(pat1)) \
                       ) \
                   ) \
               ) \
           )

/*! \brief "Greater or Equal" version comparison. */
#define FATE_VERCMP_GE(maj0, min0, pat0, maj1, min1, pat1) \
       (FATE_VERCMP_GT(maj0, min0, pat0, maj1, min1, pat1) \
     || FATE_VERCMP_EQ(maj0, min0, pat0, maj1, min1, pat1))

/*! \brief "Less Than" version comparison. */
#define FATE_VERCMP_LT(maj0, min0, pat0, maj1, min1, pat1) \
      (!FATE_VERCMP_GE(maj0, min0, pat0, maj1, min1, pat1))

/*! \brief "Less or Equal" version comparison. */
#define FATE_VERCMP_LE(maj0, min0, pat0, maj1, min1, pat1) \
       (FATE_VERCMP_LT(maj0, min0, pat0, maj1, min1, pat1) \
     || FATE_VERCMP_EQ(maj0, min0, pat0, maj1, min1, pat1))




/*
 *
 * Some more utility macros
 *
 */


/*! \brief Define <tt>__func__</tt> to an empty string, only if the used
 *         C standard does not support it. */
#if __STDC_VERSION__ < 199901L
#ifndef __GNUC__
#define __func__ ""
#else
#define __func__ __FUNCTION__
#endif
#endif

#ifndef M_PI
/*! \brief Definition for M_PI, since it may be missing with std=c11. */
#define M_PI 3.14159265358979323846264338327
#endif

/*! \brief Forces a compilation error if condition is true. Evaluates to 0. */
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))




/*
 *
 *
 * Compiler-specific macro definitions
 *
 *
 */






#if __DOXYGEN__ || defined(__GNUC__)
/*! \brief Ensures the given expression is an array.
 *
 * Always evaluates to 0, but causes a compilation error if the given 
 * expression is not an array.
 *
 * This macro expands to 0 if the compiler is not GCC.
 * \return 0
 */
#define MUST_BE_ARRAY(a) \
     BUILD_BUG_ON_ZERO(__builtin_types_compatible_p(typeof(a), typeof(&a[0])))
/*! \brief Marks a function or object as deprecated, displaying a custom 
 *         message.
 *
 * This macro expands to nothing if the compiler is not GCC. */
#define FATE_DEPRECATED(msg) __attribute__((deprecated(msg)))
/*! \brief Marks a function as printf-like.
 *
 * This gives to the compiler the ability to check formats given to the marked
 * functions.
 *
 * This macro expands to nothing if the compiler is not GCC.
 *
 * \param fmt_index The index of the "format" parameter.
 * \param args_index The index of the "..." parameter.
 */
#define FATE_PRINTF_DECL(fmt_index, args_index) \
            __attribute__((format(printf, fmt_index, args_index)))
/*! \brief Instructs the compiler that some arguments to a function cannot be 
 *         NULL. 
 *
 * This macro expands to nothing if the compiler is not GCC.
 *
 * This macro takes a variable arguments list of indices which should indicate
 * which parameters are to be non-NULL.
 */
#define FATE_NONNULL_PARAMS(arg_index,...) \
            __attribute__((nonnull(arg_index, __VA_ARGS__)))

/*! \brief Instructs the compiler that the variable arguments given to a 
 *         function must be ended by NULL. 
 *
 * This macro expands to nothing if the compiler is not GCC.
 *
 * \param pos Where must the sentinel be located, counting backwards from the 
 *            end of the arguments list ? */
#define FATE_SENTINEL(pos) __attribute__((sentinel(pos)))
/*! \brief Instructs the compiler that the function's result should not be
 *         ignored. 
 *
 * This macro includes \c sal.h and expands to \c _Check_return_ if the
 * compiler is MSVC.
 */
#define FATE_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
/*! \brief Asks the compiler to reduce the size taken by instances of the
 *         marked struct as much as possible. 
 *
 * This macro expands to nothing if the compiler is not GCC.
 */
#define FATE_PACKED_STRUCT __attribute__((packed))

#else /* ifdef __GNUC__ */

#define MUST_BE_ARRAY(a) 0
#define FATE_DEPRECATED(msg) 
#define FATE_PRINTF_DECL(fmt_index, args_index) 
#define FATE_NONNULL_PARAMS(arg_index,...) 
#define FATE_SENTINEL(pos) 
#define FATE_WARN_UNUSED_RESULT 
#define FATE_PACKED_STRUCT

#endif /* ifdef __GNUC_ */

#ifdef _MSC_VER
#include <sal.h>
#undef  FATE_WARN_UNUSED_RESULT
#define FATE_WARN_UNUSED_RESULT _Check_return_
#endif


/*! \brief Returns the number of elements of an array which size can be 
 *         computed at compile-time. 
 * 
 * Please don't use it on pointers. It ain't wizardry.
 */
#define COUNTOF(arr) (sizeof(arr)/sizeof((arr)[0]) + MUST_BE_ARRAY(arr))

/*! @} */

#endif /* FATE_DEFS_H */
