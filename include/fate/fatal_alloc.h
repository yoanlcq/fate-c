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

/*! \file fate/fatal_alloc.h
 *  \brief Exit-on-failure dynamic memory allocaton routines.
 *
 * TODO
 */


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
