#ifndef FATE_DEFS_H
#define FATE_DEFS_H

#define FATE_VERSION_MAJOR 0
#define FATE_VERSION_MINOR 1
#define FATE_VERSION_PATCH 0
#define FATE_VERSION_CODENAME "Rainbow Dice"

#define XSTRINGIFY(X) STRINGIFY(X)
#define STRINGIFY(X) #X

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

#if __STDC_VERSION__ < 199901L
#define __func__ ""
#endif

#if defined(_WIN32) || defined(__WIN32__)
    #define FATE_DEFS_WINDOWS
#elif defined(linux) || defined(__linux)
    #define FATE_DEFS_LINUX
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) ||            defined(Macintosh)
    #define FATE_DEFS_OSX
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
    #define FATE_DEFS_FREEBSD
#else
    #error This operating system is not supported by F.A.T.E.
#endif

#ifdef FATE_DEFS_WINDOWS
    #ifdef _WIN32_WINNT
        #if _WIN32_WINNT < 0x0600
            #undef _WIN32_WINNT
            #define _WIN32_WINNT 0x0600
        #endif
    #else
        #define _WIN32_WINNT 0x0600
    #endif
#endif

#include <fate/log.h>

#define FATE_DEFS_STACKTRACE_FRAMES_CAPACITY 128
#define FATE_DEFS_STACKTRACE_MODULE_NAME_CAPACITY 256

/* You know, in case it changes in 2040. */
#define FATE_DEFS_DEFAULT_REFRESH_RATE 60


#endif /* FST_DEFS_H */
