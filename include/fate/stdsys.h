#ifndef FE_STDSYS_H
#define FE_STDSYS_H

#include <fate/defs.h>

#ifdef FE_TARGET_EMSCRIPTEN
    #include <emscripten.h>
#endif

#if defined(FE_TARGET_WINDOWS)
#define NOMINMAX
    #include <Windows.h>
    #include <Winerror.h>
    #include <DbgHelp.h>
    #include <Shellapi.h>
    #include <Shlobj.h>
    #include <Objbase.h>
    #define INITGUID
    #include <KnownFolders.h>
    #undef INITGUID
/* http://lolengine.net/blog/2011/3/4/fuck-you-microsoft-near-far-macros */
    #ifdef near
    #undef near
    #endif
    #ifdef far
    #undef far
    #endif
    #ifdef small
    #undef small
    #endif
#else
    #include <errno.h>
    #include <limits.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/mman.h>
    #include <sys/uio.h>
    #include <signal.h>
    #include <fcntl.h>
    #ifndef FE_TARGET_ANDROID
        #include <execinfo.h>
    #endif
#endif

#if defined(FE_TARGET_FREEBSD)
    #include <sys/sysctl.h>
#elif defined(FE_TARGET_OSX) || defined(FE_TARGET_IOS)
    #include <mach-o/dyld.h>
    #include <sys/param.h>
    #include <sys/mount.h>
#endif




#endif /* FE_STDSYS_H */
