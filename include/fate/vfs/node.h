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

#ifndef FATE_NODE_H
#define FATE_NODE_H

#include <stdint.h>

enum fate_node_pool_id {
    FATE_NODE    = 0,
    FATE_SYMLINK = 1,
    FATE_ANODE   = 2, /* Anonymous node. */
    FATE_LNODE   = 3, /* Local device node. */
    FATE_KNODE   = 4, /* Value node. (Derived from 'Key') */
    FATE_DNODE   = 5, /* Entity Data node. */
    FATE_ENODE   = 6, /* Entity instance node. */
    FATE_VNODE   = 7, /* View node. */
    FATE_SNODE   = 8, /* State node (function pointer). */
    FATE_RNODE   = 9, /* Region nodes. */
};
typedef enum fate_node_pool_id fate_node_pool_id;

typedef uint8_t fate_node_links_count; /* flexible */
/* Should be of 32 bits. */
struct fate_node_metadata {
    fate_node_links_count  links_count;
    fate_node_pool_id      pool_id : 6;
    unsigned       is_readonly : 1
    unsigned       is_shared   : 1;
    unsigned       reserved    : 9;
};

#define FATE_NODE_ATTRIBUTES \
    FATE_LEAF_ATTRIBUTES; \
    fate_dentry     *dentries; \
    unsigned     num_dentries

struct fate_symlink {
    fate_node_metadata metadata;
    uint8_t *target_path;
};
typedef struct fate_symlink fate_symlink;

typedef uint16_t fate_dentry_count; /* flexible */
typedef uint16_t fate_node_data_index; /* flexible */
struct fate_node {
    fate_node_metadata metadata;
    fate_node_data_index index;
    fate_dentry *dentries;
    fate_dentry_count dentries_count;
};
typedef struct fate_node fate_node;

struct fate_dentry {
    uint8_t   *name;
    /* fate_dentry_hash hash; instead of name, maybe ? */
    fate_node *node;
};
typedef struct fate_dentry fate_dentry;


#endif /* FATE_NODE_H */
