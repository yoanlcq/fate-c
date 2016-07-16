#include <fate/mem/stack.h>

#ifdef FE_TARGET_LINUX

#include <stdio.h> /* sprintf() */
#include <stdlib.h> /* strtoul() */
#include <stdbool.h>
#include <stdint.h> /* uintptr_t */
#include <assert.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


typedef struct {
    void *bottom, *pointer;
} stack_ptrs;

typedef struct {
    unsigned long soft, hard;
} stack_rlim;


static bool stack_rlim_refresh(stack_rlim *srlim) {
    struct rlimit rlim;
    int ret = getrlimit(RLIMIT_STACK, &rlim);
    if(ret == -1)
        return false;
    srlim->soft = rlim.rlim_cur;
    srlim->hard = rlim.rlim_max;
    return true;
}


#ifdef _GNU_SOURCE
#include <sys/syscall.h>
static pid_t gettid(void) { return syscall(SYS_gettid); }
#define HAS_GETTID
#endif /* _GNU_SOURCE */


#ifndef HAS_GETTID
FE_WARN_IF_USED("\n    Since _GNU_SOURCE is not defined, the gettid() "
        "system call is unavailable.\n"
        "    This function won't open \"/proc/self/task/<tid>/stat\",\n"
        "    and therefore will fail immediately.\n"
        "    fe_mem_stackavail() will return FE_MEM_STACKAVAIL_UNKNOWN.")
#endif
static bool stack_ptrs_refresh(stack_ptrs *sptrs) {
#ifndef HAS_GETTID
    return false;
#else
    /* On my laptop, a size of approximately 256 bytes does the trick,
     * but let's be safer.
     * Also, it sound quite dumb to reserve such a buffer on the stack while
     * we're actually asking how much space is left on it. I've found no
     * workarounds so far.
     * - ioctl(FIONREAD) yields 0;
     * - fstat() reports a size of 0;
     */
    char buf[1024];
    sprintf(buf, "/proc/self/task/%u/stat", (unsigned)gettid());
    int fd = open(buf, O_RDONLY, 0666);
    if(fd == -1)
        return false;

    ssize_t bytes_read;
retry_read:
    bytes_read = read(fd, buf, sizeof buf);
    if(bytes_read == -1) {
        if(errno == EINTR)
            goto retry_read;
        else {
            close(fd);
            return false;
        }
    }
    assert(bytes_read > 0);
    close(fd);

    /* I'm doing this because an executable name can legitimately contain
     * parentheses and spaces, so it is NOT safe to scanf("%s") as advertised
     * by proc(5). What we are certain of, is that the string is wrapped in
     * parentheses - so the last closing parenthese is where we can start
     * parsing. 
     */
    const char *cur = buf + bytes_read - 1;
    do --cur; while(*cur != ')');
    cur += 4;

    size_t fieldno;
    for(fieldno=4 ; fieldno<28 ; ++fieldno) {
        do ++cur; while(*cur != ' ');
        ++cur;
    }

    sptrs->bottom  = (void*)(uintptr_t) strtoul(cur, NULL, 10);
#ifdef __GNUC__
    /* Well uh I guess that does it. */
    sptrs->pointer = __builtin_frame_address(0);
#else
    do ++cur; while(*cur != ' ');
    ++cur;
    /* Very naive approximation. */
    sptrs->pointer = (void*)(uintptr_t) strtoul(cur, NULL, 10);
    const size_t remove = sizeof(buf) + sizeof(fd) + sizeof(bytes_read) 
                        + sizeof(cur) + sizeof(fieldno) + sizeof(sptrs);
#if FE_MEM_STACK_GROWTH_DIRECTION < 0
    sptrs->pointer -= remove;
#elif FE_MEM_STACK_GROWTH_DIRECTION > 0
    sptrs->pointer += remove;
#else
    sptrs->pointer += sptrs->pointer > stack->bottom ? -remove : remove;
#endif
#endif /* __GNUC__ */

    return true;
#endif /* HAS_GETTID */
}


/* See the public API doc. I really wish I could understand how to accurately
 * compute this value instead. */
#define STACK_SAFETY_MARGIN (1ul<<14ul)

fe_mem_stackavail_status fe_mem_stackavail(size_t *available) {
    stack_ptrs sptrs = {0};
    stack_rlim srlim = {0};

    stack_ptrs_refresh(&sptrs);
    stack_rlim_refresh(&srlim);

    const size_t safety_margin = STACK_SAFETY_MARGIN;
    const size_t naive_used
#if FE_MEM_STACK_GROWTH_DIRECTION < 0
        = sptrs.bottom - sptrs.pointer;
#elif FE_MEM_STACK_GROWTH_DIRECTION > 0
        = sptrs.pointer - sptrs.bottom;
#else
        = sptrs.pointer > sptrs.bottom
        ? sptrs.pointer - sptrs.bottom
        : sptrs.bottom  - sptrs.pointer;
#endif
    if(!naive_used)
        return FE_MEM_STACKAVAIL_UNKNOWN;
    const size_t naive_avail = srlim.soft > naive_used
                             ? srlim.soft - naive_used : 0;
    *available = naive_avail > safety_margin
               ? naive_avail - safety_margin : 0;
    return FE_MEM_STACKAVAIL_OK;
}

#else
#error "TODO: other systems"
#endif /* FE_TARGET_LINUX */

