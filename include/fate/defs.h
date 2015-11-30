#ifndef FATE_DEFS_H
#define FATE_DEFS_H

#include <fate/log.h>

#define XSTRINGIFY(X) STRINGIFY(X)
#define STRINGIFY(X) #X

#if __STDC_VERSION__ < 199901L
#define __func__ ""
#endif

#if defined(FATE_DEBUG_BUILD)
#define inline
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
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#endif

#ifdef FATE_DEBUG_BUILD
#define FATE_CHECK_MALLOC(X) \
    if((X)==NULL) { \
        fate_logf_err("%s:%u:%s Dynamic allocation failed !\n", \
                      __FILE__, __LINE__, __func__); \
        exit(EXIT_FAILURE); \
    }
#else
#define FATE_CHECK_MALLOC(X) (X)
#endif


#define FATE_DEFS_STACKTRACE_FRAMES_CAPACITY 128
#define FATE_DEFS_STACKTRACE_MODULE_NAME_CAPACITY 256



/* If this macro is set, GNU extensions are used when they perform 
 * better than otherwise naïve code. 
 * Our source must still provide fallback code. */
#define _GNU_SOURCE

/* You know, in case it changes in 2040. */
#define FATE_DEFS_DEFAULT_REFRESH_RATE 60

/* Node names only take the needed amount of memory (not fixed-sized buffers).
 * However, this defines the static buffer's size used to hold a name in 
 * the path resolution process.
 * fst_checkpath() reports any node name in a path which length exceeds 
 * this size. */
#define FATE_DEFS_NODE_NAME_MAX_LENGTH 255

/* By default, fst's path resolution routines don't check if any node name's
 * length exceeds the limit defined above (leaving it up to fst_checkpath()). 
 * Defining the macro below forces them to do so. 
 * Since it has a negative impact on speed, the user is advised to comment 
 * it out when their game is ready for release. */
#define FATE_DEFS_CHECK_NODE_NAME_OVERFLOW

/* This defines the static stack's capacity used to hold the absolute 
 * full path to the Current Working Node (CWN) in the path resolution process.
 * Therefore, it also defines the Global Tree's maximum depth.
 * It has no impact on speed, only space.
 * This macro's value must be in the range of an unsigned 32-bit integer. */
#define FATE_DEFS_MAX_PATH_STACK 255

/* This defines the integer size to be used for fst_nodepack's count field.
 * fst_nodepacks are used to store a node's children.
 * Using a smaller value saves memory, but also limits the number of 
 * children any given node can have to (2^n-1), where n is this macro's value.
 * It also affects the fst_nodepack functions' memory allocation strategy 
 * and behaviour.
 * The user is advised to adapt this value to their needs.
 * This macro's value can be set to either 8, 16, 32 or 64. */
#define FATE_DEFS_DENTRY_CAPACITY_INTSIZE 16


#endif /* FST_DEFS_H */
