#ifndef FE_STDSYS_H
#define FE_STDSYS_H

#include <fate/defs.h>

#ifdef FE_TARGET_EMSCRIPTEN
    #include <emscripten.h>
#endif

#if defined(FE_TARGET_WINDOWS)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <Windows.h>
    #include <Winerror.h>
    #include <DbgHelp.h>
    #include <Shellapi.h>
    #include <Shlobj.h>
    #include <Objbase.h>
    #define INITGUID
    #include <KnownFolders.h>
    #undef INITGUID
/* 
 * http://lolengine.net/blog/2011/3/4/fuck-you-microsoft-near-far-macros 
 *
 * The worse is that we can't do anything about it. Undefining them screws
 * MSVC headers which rely on them.
 * So I'm leaving this here so I'm not doing that error again.
 */
/*
    #ifdef near
    #undef near
    #endif
    #ifdef far
    #undef far
    #endif
    #ifdef small
    #undef small
    #endif
    */
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
    #ifndef FE_TARGET_EMSCRIPTEN
        #include <execinfo.h>
    #endif
    #endif
#endif

#if defined(FE_TARGET_FREEBSD)
    #include <sys/sysctl.h>
#elif defined(FE_TARGET_OSX) || defined(FE_TARGET_IOS)
    #include <mach-o/dyld.h>
    #include <sys/param.h>
    #include <sys/mount.h>
#endif

#if !defined(FE_TARGET_WINDOWS) \
 && !(defined(FE_TARGET_OSX) || defined(FE_TARGET_IOS))
#include <dlfcn.h>
#endif


#endif /* FE_STDSYS_H */
