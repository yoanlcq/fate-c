#ifndef FATE_FATAL_ALLOC_H
#define FATE_FATAL_ALLOC_H

#include <stddef.h>
#include <fate/defs.h>

#ifdef FATE_DEBUG_BUILD
extern void* fate_fatal_malloc(size_t nmemb, size_t size);
extern void* fate_fatal_calloc(size_t nmemb, size_t size);
extern void* fate_fatal_realloc(void *ptr, size_t nmemb, size_t size);
#else
#define fate_fatal_malloc(_nmemb_,_size_) malloc(_nmemb_*_size_)
#define fate_fatal_calloc(_nmemb_,_size_) calloc(_nmemb_,_size_)
#define fate_fatal_realloc(_ptr_,_nmemb_,_size_) realloc(_ptr_,_nmemb_*_size_)
#endif

#endif /* FATE_FATAL_ALLOC_H */
