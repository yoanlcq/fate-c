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

#ifndef FE_DEFS_H
#define FE_DEFS_H

/*! \brief Stringify the expansion of the given expression. */
#define XSTRINGIFY(X) STRINGIFY(X)

/*! \brief Stringify the given expression. */
#define STRINGIFY(X) #X

#define KIBIBYTES(bytes) (1024*(bytes))
#define MEBIBYTES(bytes) (1024*KIBIBYTES(bytes))
#define GIBIBYTES(bytes) (1024*MEBIBYTES(bytes))


/*! \brief The current F.A.T.E version's codename. 
 *
 * It serves no other purpose than being a fun way of identifying a version. 
 */
#define FE_VERSION_CODENAME "Predicator"
/*! \brief The current F.A.T.E version's prerelease string, conforming to 
 *         point 9 of
 *         <a href="http://semver.org/spec/v2.0.0.html" target="_blank">
 *         the Semantic Versioning 2.0.0 Specification</a>.
 *
 * Define it to something like "-alpha", "-beta",
 * or define it to the empty string.
 */
#define FE_VERSION_PRERELEASE_STR
#define FE_VERSION_MAJOR 0
#define FE_VERSION_MINOR 2
#define FE_VERSION_PATCH 0
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
#ifndef FE_VERSION_BUILD_METADATA
#define FE_VERSION_BUILD_METADATA
#endif

/*! \brief The complete string identifying F.A.T.E's current version. */
#define FE_VERSION \
    XSTRINGIFY(FE_VERSION_MAJOR) "." XSTRINGIFY(FE_VERSION_MINOR) "." \
    XSTRINGIFY(FE_VERSION_PATCH) FE_VERSION_PRERELEASE_STR \
    FE_VERSION_BUILD_METADATA


/*! \brief Compiler/platform-specific "__declspec(dllexport)". */
#define FE_EXPORT

#if __DOXYGEN__
/*! \brief Defined only if the target is Windows. */
#define FE_TARGET_WINDOWS 1
/*! \brief Defined only if the target is Linux. */
#define FE_TARGET_LINUX 1
/*! \brief Defined only if the target is Mac OS X. */
#define FE_TARGET_OSX 1
/*! \brief Defined only if the target is FreeBSD. */
#define FE_TARGET_FREEBSD 1
/*! \brief Defined only if the target is Android. */
#define FE_TARGET_ANDROID 1
/*! \brief Defined only if the target is the iOS Simulator. */
#define FE_TARGET_IOS_SIMULATOR 1
/*! \brief Defined only if the target is either iOS or the iOS Simulator. */
#define FE_TARGET_IOS 1
/*! \brief Defined only if the target is Emscripten. */
#define FE_TARGET_EMSCRIPTEN 1
#endif

#if defined(__ANDROID__)
    #define FE_TARGET_ANDROID 1
#endif
#if defined(_WIN32) || defined(__WIN32__)
    #define FE_TARGET_WINDOWS 1
#endif
#if defined(linux) || defined(__linux)
    #define FE_TARGET_LINUX 1
#endif
/*
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) \
      || defined(Macintosh)
    #define FE_OSX
*/
#if __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
        #define FE_TARGET_IOS_SIMULATOR 1
        #define FE_TARGET_IOS 1
    #elif TARGET_OS_IPHONE
        #define FE_TARGET_IOS 1
    #elif TARGET_OS_MAC
        #define FE_TARGET_OSX 1
    #else
        #error "Unknown Apple platform."
    #endif
#endif
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
    #define FE_TARGET_FREEBSD 1
#endif
#if defined(__EMSCRIPTEN__)
    #define FE_TARGET_EMSCRIPTEN 1
#endif

#if !defined(FE_TARGET_LINUX) \
 && !defined(FE_TARGET_WINDOWS) \
 && !defined(FE_TARGET_OSX) \
 && !defined(FE_TARGET_FREEBSD) \
 && !defined(FE_TARGET_ANDROID) \
 && !defined(FE_TARGET_IOS_SIMULATOR) \
 && !defined(FE_TARGET_IOS) \
 && !defined(FE_TARGET_EMSCRIPTEN) 
    #error This platform is not supported by F.A.T.E.
#endif

/*! \brief Is the target a Unix ? */
#if __DOXYGEN__ \
 || defined(FE_TARGET_LINUX) \
 || defined(FE_TARGET_OSX) \
 || defined(FE_TARGET_FREEBSD) \
 || defined(FE_TARGET_ANDROID) \
 || defined(FE_TARGET_IOS_SIMULATOR) \
 || defined(FE_TARGET_IOS)
#define FE_TARGET_IS_A_UNIX 1
#endif

#if __DOXYGEN__
#define _WIN32_WINNT 0x0600
#endif

#if FE_TARGET_WINDOWS

    #define WIN32_LEAN_AND_MEAN

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
#define FE_VERCMP_EQ(maj0, min0, pat0, maj1, min1, pat1) \
            ((maj0)==(maj1) && (min0)==(min1) && (pat0)==(pat1))

/*! \brief "Greater Than" version comparison. */
#define FE_VERCMP_GT(maj0, min0, pat0, maj1, min1, pat1) \
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
#define FE_VERCMP_GE(maj0, min0, pat0, maj1, min1, pat1) \
       (FE_VERCMP_GT(maj0, min0, pat0, maj1, min1, pat1) \
     || FE_VERCMP_EQ(maj0, min0, pat0, maj1, min1, pat1))

/*! \brief "Less Than" version comparison. */
#define FE_VERCMP_LT(maj0, min0, pat0, maj1, min1, pat1) \
      (!FE_VERCMP_GE(maj0, min0, pat0, maj1, min1, pat1))

/*! \brief "Less or Equal" version comparison. */
#define FE_VERCMP_LE(maj0, min0, pat0, maj1, min1, pat1) \
       (FE_VERCMP_LT(maj0, min0, pat0, maj1, min1, pat1) \
     || FE_VERCMP_EQ(maj0, min0, pat0, maj1, min1, pat1))




/*
 *
 * Some more utility macros
 *
 */


/*! \brief Macro for checking if we're using the C11 standard. */
#if __STDC_VERSION__ >= 201112L
#define FE_C11_SUPPORT
#endif


/*! \brief Define <tt>__func__</tt> to an empty string, only if the used
 *         C standard does not support it. */
#if __STDC_VERSION__ < 199901L
#ifndef __GNUC__
#define __func__ ""
#endif
#endif

#ifdef _MSC_VER
#define restrict __restrict
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
#define FE_DEPRECATED(msg) __attribute__((deprecated(msg)))
/*! \brief Marks a function as malloc-like.
 *
 * This macro expands to nothing if the compiler is not GCC.
 */
#define FE_MALLOC_DECL __attribute__((malloc))
/*! \brief Marks a function as printf-like.
 *
 * This gives to the compiler the ability to check formats given to the marked
 * functions.
 *
 * This macro expands to nothing if the compiler is not GCC.
 *
 * \param fmt_index The index of the "format" parameter, starting from 1.
 * \param args_index The index of the "..." parameter, starting from 1.
 */
/* We say __printf__ instead of printf because libintl.h may redefine printf as 
 * _libintl_printf. */
#define FE_PRINTF_DECL(fmt_index, args_index) \
            __attribute__((format(__printf__, fmt_index, args_index)))
/*! \brief Instructs the compiler that some arguments to a function cannot be 
 *         NULL. 
 *
 * This macro expands to nothing if the compiler is not GCC.
 *
 * This macro takes a variable arguments list of indices which should indicate
 * which parameters are to be non-NULL.
 */
#define FE_NONNULL_PARAMS(arg_index,...) \
            __attribute__((nonnull(arg_index, __VA_ARGS__)))

/*! \brief Instructs the compiler that the variable arguments given to a 
 *         function must be ended by NULL. 
 *
 * This macro expands to nothing if the compiler is not GCC.
 *
 * \param pos Where must the sentinel be located, counting backwards from the 
 *            end of the arguments list ? */
#define FE_SENTINEL(pos) __attribute__((sentinel(pos)))
/*! \brief Instructs the compiler that the function's result should not be
 *         ignored. 
 *
 * This macro includes \c sal.h and expands to \c _Check_return_ if the
 * compiler is MSVC.
 */
#define FE_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
/*! \brief Asks the compiler to reduce the size taken by instances of the
 *         marked struct as much as possible. 
 *
 * This macro expands to nothing if the compiler is not GCC.
 */
#define FE_PACKED_STRUCT __attribute__((packed))
/*! \brief Expands to __attribute__((warning(...))) if supported.
 *
 * This macro expands to nothing if the compiler is not GCC.
 */
#define FE_WARN_IF_USED(str) __attribute__((warning(str)))
#ifdef __clang__
#undef FE_WARN_IF_USED
#define FE_WARN_IF_USED(str)
#endif
/*! \brief Marks a function as "NOT IMPLEMENTED YET"
 *
 * Use this for functions that are at most 50% finished.
 * This macro expands to nothing if the compiler is not GCC.
 */
#define FE_NIY FE_WARN_IF_USED("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~NOT IMPLEMENTED YET~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")
/*! \brief Marks a function as "WORK IN PROGRESS"
 *
 * Use this for functions that are at least 50% finished.
 * This macro expands to nothing if the compiler is not GCC.
 */
#define FE_WIP FE_WARN_IF_USED("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~WORK IN PROGRESS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")


#else /* ifdef __GNUC__ */

#define MUST_BE_ARRAY(a) 0
#define FE_DEPRECATED(msg) 
#define FE_MALLOC_DECL
#define FE_PRINTF_DECL(fmt_index, args_index) 
#define FE_NONNULL_PARAMS(arg_index,...) 
#define FE_SENTINEL(pos) 
#define FE_WARN_UNUSED_RESULT 
#define FE_PACKED_STRUCT
#define FE_WARN_IF_USED(str)
#define FE_NIY
#define FE_WIP

#endif /* ifdef __GNUC_ */

/*! \brief Compile-time assert macro. */
#ifdef _MSC_VER
#define FE_COMPILETIME_ASSERT(pred, str) static_assert(pred, str)
#elif defined(FE_C11_SUPPORT)
#define FE_COMPILETIME_ASSERT(pred, str) _Static_assert(pred, str)
#else
#define FE_COMPILETIME_ASSERT(pred, str) \
    typedef char static_assertion_failed_at_line_##__LINE__[(pred && str)*2-1]
#endif

/*! \brief Compile-time specifier for a declaration (not a typedef) 
 *         requiring a specific alignment.
 *
 * This matters when using some low-level instructions such as _mm_stream_si128().
 */
#ifdef FE_C11_SUPPORT
#include <stdalign.h>
#define FE_ALIGN(n) _Alignas(n)
#elif defined(__GNUC__)
#define FE_ALIGN(n) __attribute__((align(n)))
#elif defined(_MSC_VER)
#define FE_ALIGN(n) __declspec(align(n))
#else
#error "FE_ALIGN() can't be defined because your compiler doesn't support alignment specifiers."
#endif


#ifdef _MSC_VER
#include <sal.h>
#undef  FE_WARN_UNUSED_RESULT
#define FE_WARN_UNUSED_RESULT _Check_return_
#endif


/*! \brief Returns the number of elements of an array which size can be 
 *         computed at compile-time. 
 * 
 * Please don't use it on pointers. It ain't wizardry.
 */
#define COUNTOF(arr) (sizeof(arr)/sizeof((arr)[0]) + MUST_BE_ARRAY(arr))


typedef unsigned long ulong;

#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#ifdef FE_TARGET_WINDOWS
#define PRIssize_t "Id"
#define PRIsize_t "Iu"
#define PRIosize_t "Io"
#define PRIxsize_t "Ix"
#define PRIXsize_t "IX"
#else
#define PRIssize_t "zd"
#define PRIsize_t "zu"
#define PRIosize_t "zo"
#define PRIxsize_t "zx"
#define PRIXsize_t "zX"
#endif

/*! @} */

#endif /* FE_DEFS_H */
