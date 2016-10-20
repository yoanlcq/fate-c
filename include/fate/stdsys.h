#ifndef FE_STDSYS_H
#define FE_STDSYS_H

#include <fate/defs.h>

#ifdef FE_TARGET_EMSCRIPTEN
    #include <emscripten.h>
#endif

#if defined(FE_TARGET_WINDOWS)
    #include <Windows.h>
    #include <Shellapi.h>
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
#endif

#if defined(FE_TARGET_FREEBSD)
    #include <sys/sysctl.h>
#elif defined(FE_TARGET_OSX) || defined(FE_TARGET_IOS)
    #include <mach-o/dyld.h>
    #include <sys/param.h>
    #include <sys/mount.h>
#endif



#endif /* FE_STDSYS_H */
