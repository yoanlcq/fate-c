#include <fate/defs.h>
#include <fate/fatal_alloc.h>

#ifdef FATE_DEBUG_BUILD

#include <stdlib.h>
#include <fate/log.h>
#include <fate/sys.h>

void* fate_fatal_malloc(size_t nmemb, size_t size) {
    void *ptr = malloc(nmemb*size);
    if(ptr)
        return ptr;
    fate_logf_err("Fatal: Unhandled %salloc() failure.\n", "m");
    fate_logf_err("nmemb = %lu\nsize  = %lu\n", nmemb, size);
    fate_logf_err("Stack trace :\n");
    fate_sys_log_stacktrace(fate_logf_err);
    exit(EXIT_FAILURE);
    return NULL;
}
void* fate_fatal_calloc(size_t nmemb, size_t size) {
    void *ptr = calloc(nmemb, size);
    if(ptr)
        return ptr;
    fate_logf_err("Fatal: Unhandled %salloc() failure.\n", "c");
    fate_logf_err("nmemb = %lu\nsize  = %lu\n", nmemb, size);
    fate_logf_err("Stack trace :\n");
    fate_sys_log_stacktrace(fate_logf_err);
    exit(EXIT_FAILURE);
    return NULL;
}
void* fate_fatal_realloc(void *ptr, size_t nmemb, size_t size) {
    unsigned i;
    void *newptr = realloc(ptr, nmemb*size);
    if(newptr)
        return newptr;
    fate_logf_err("Fatal: Unhandled %salloc() failure.\n", "re");
    fate_logf_err("ptr   = %p\n", ptr);
    if(ptr) {
        fate_logf_err("*ptr  = ");
        for(i=0 ; i<size ; ++i)
            fate_logf_err("%2x", ((char*)ptr)[i]);
    }
    fate_logf_err("nmemb = %lu\nsize  = %lu\n", nmemb, size);
    fate_logf_err("Stack trace :\n");
    fate_sys_log_stacktrace(fate_logf_err);
    exit(EXIT_FAILURE);
    return NULL;
}
#endif

