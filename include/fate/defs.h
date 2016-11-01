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
//#define FE_VERSION_CODENAME "Predicator"
#define FE_VERSION_CODENAME "Gatherer"
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
/*! \brief Defined only if the target is Windows RT. */
#define FE_TARGET_WINDOWS_RT 1
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
 && !defined(FE_TARGET_WINDOWS_RT) \
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

#if __DOXYGEN__ \
 || defined(FE_TARGET_WINDOWS) \
 || defined(FE_TARGET_OSX) \
 || (defined(FE_TARGET_LINUX) && !defined(FE_TARGET_ANDROID)) \
 || defined(FE_TARGET_FREEBSD)
#define FE_TARGET_ENV_DESKTOP 1
#endif

#if __DOXYGEN__ \
 || defined(FE_TARGET_ANDROID) \
 || defined(FE_TARGET_IOS) \
 || defined(FE_TARGET_IOS_SIMULATOR)
#define FE_TARGET_ENV_MOBILE 1
#endif

#if __DOXYGEN__ || defined(FE_TARGET_EMSCRIPTEN)
#define FE_TARGET_ENV_BROWSER 1
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
 * Compiler detection macros
 *
 *
 */

#ifdef __GNUC__
    #define FE_CC_GCC_COMPATIBLE
    #ifdef __clang__
        #define FE_CC_CLANG
    #else
        #define FE_CC_GCC
    #endif
#elif defined(_MSC_VER)
    #define FE_CC_MSVC
#else
#error "This compiler was not expected. For now, we stick to GCC, Clang and MSVC."
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


#ifdef FE_CC_MSVC
#define fe_pragma_message(msg) __pragma(message(msg))
#elif defined(FE_CC_GCC_COMPATIBLE)
#define fe_pragma_message(msg) _Pragma(XSTRINGIFY(message #msg))
#else
#error "Wait! Don't know how to define fe_pragma_message() here."
#define fe_pragma_message(msg)
#endif

#ifdef FE_CC_MSVC
    #ifdef FE_DEBUG_BUILD
        #define fe_pragma_optimize_speed() __pragma(optimize("gt", on))
        #define fe_pragma_optimize_size()  __pragma(optimize("gs", on))
    #else
        #define fe_pragma_optimize_speed() __pragma(optimize("ygt", on))
        #define fe_pragma_optimize_size()  __pragma(optimize("ygs", on))
    #endif
    #define     fe_pragma_optimize_reset() __pragma(optimize("", on))
#elif defined(FE_CC_GCC)
    #ifdef FE_DEBUG_BUILD
        #define fe_pragma_optimize_speed() _Pragma(XSTRINGIFY(GCC optimize("Og")))
        #define fe_pragma_optimize_size()  _Pragma(XSTRINGIFY(GCC optimize("Og")))
    #else
        #define fe_pragma_optimize_speed() _Pragma(XSTRINGIFY(GCC optimize("O3")))
        #define fe_pragma_optimize_size()  _Pragma(XSTRINGIFY(GCC optimize("Os")))
    #endif
    #define     fe_pragma_optimize_reset() _Pragma(XSTRINGIFY(GCC reset_options))
#elif defined(FE_CC_CLANG)
/* Nothing here, it's fine. */
#define fe_pragma_optimize_speed()
#define fe_pragma_optimize_size()
#define fe_pragma_optimize_reset()
#else
#error "Wait! Don't know how to define fe_pragma_optimize_for_speed() here."
#define fe_pragma_optimize_speed()
#define fe_pragma_optimize_size()
#define fe_pragma_optimize_reset()
#endif


/* Note : Only include these last.
 * They need the FE_ platform macros. */
#include <fate/decl.h>
#include <fate/stdc.h>
#include <fate/stdc_ext.h>



/*! @} */

#endif /* FE_DEFS_H */
