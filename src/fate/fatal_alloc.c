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

/*! \file fate/fatal_alloc.c
 *  \brief Exit-on-failure dynamic memory allocation routines.
 *
 * TODO
 */

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
    fate_logf_err(_("Fatal: Unhandled %salloc() failure.\n"), "m");
    fate_logf_err("nmemb = %lu\nsize  = %lu\n", nmemb, size);
    fate_logf_err(_("Stack trace :\n"));
    fate_sys_log_stacktrace(fate_logf_err);
    exit(EXIT_FAILURE);
    return NULL;
}
void* fate_fatal_calloc(size_t nmemb, size_t size) {
    void *ptr = calloc(nmemb, size);
    if(ptr)
        return ptr;
    fate_logf_err(_("Fatal: Unhandled %salloc() failure.\n"), "c");
    fate_logf_err("nmemb = %lu\nsize  = %lu\n", nmemb, size);
    fate_logf_err(_("Stack trace :\n"));
    fate_sys_log_stacktrace(fate_logf_err);
    exit(EXIT_FAILURE);
    return NULL;
}
void* fate_fatal_realloc(void *ptr, size_t nmemb, size_t size) {
    unsigned i;
    void *newptr = realloc(ptr, nmemb*size);
    if(newptr)
        return newptr;
    fate_logf_err(_("Fatal: Unhandled %salloc() failure.\n"), "re");
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

