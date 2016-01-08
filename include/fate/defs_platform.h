#ifndef FATE_DEFS_PLATFORM_H
#define FATE_DEFS_PLATFORM_H

#if defined(_WIN32) || defined(__WIN32__)
    #define FATE_WINDOWS
#elif defined(linux) || defined(__linux)
    #define FATE_LINUX
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) ||            defined(Macintosh)
    #define FATE_OSX
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
    #define FATE_FREEBSD
#elif defined(__EMSCRIPTEN__)
    #define FATE_EMSCRIPTEN
#else
    #error This platform is not supported by F.A.T.E.
#endif

#if FATE_WINDOWS
    #ifdef _WIN32_WINNT
        #if _WIN32_WINNT < 0x0600
            #undef _WIN32_WINNT
            #define _WIN32_WINNT 0x0600
        #endif
    #else
        #define _WIN32_WINNT 0x0600
    #endif
#endif

#endif /* FATE_DEFS_PLATFORM_H */
