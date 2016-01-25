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

#ifndef FST_NODES_H
#define FST_NODES_H

#include <stdio.h>
#include <stdint.h>
#include <fst/node.h>

void fst_anynode_printl(FILE *stream, fst_node *n);
void fst_anynode_dealloc(fst_node *n, uint8_t depth);

struct fst_nodepack {
    fst_node **nodes; /* Array of node addresses. */
    uintFST_CFG_NODEPACK_CAPACITY_INTSIZE_t count;
};
typedef struct fst_nodestack fst_nodestack;

inline void fst_nodestack_push            (fst_nodestack *s, fst_node *n);
inline void fst_nodestack_static_push     (fst_nodestack *s, fst_node *n);
inline fst_node *fst_nodestack_pop        (fst_nodestack *s);
inline fst_node *fst_nodestack_static_pop (fst_nodestack *s);
inline fst_node *fst_nodestack_top        (fst_nodestack *s);
inline void fst_nodestack_clear           (fst_nodestack *s);
#define     fst_nodestack_static_clear(__s__) __s__->count = 0;
void fst_nodestack_dump(FILE *stream, fst_nodestack *s, unsigned from_top, unsigned from_bot);

struct fst_nodequeue {
    fst_node **nodes;
    uint_fast32_t count;
    uint_fast32_t index;
};

void fst_nodequeue_push (fst_nodequeue *q, fst_node *n);
fst_node* fst_nodequeue_pop  (fst_nodequeue *q);
#define fst_nodequeue_top(__q__) \
    ((__q__)->count ? (__q__)->nodes[(__q__)->index] : NULL)
#define fst_nodequeue_clear(__q__) \
    if((__q__)->count) { \
        free((__q__)->nodes); \
        (__q__)->count = 0; \
        (__q__)->index = 0; \
    }

#endif /* FST_NODES_H */
