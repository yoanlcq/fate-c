#ifndef FATE_NODE_H
#define FATE_NODE_H

#include <stdint.h>

enum fate_node_type {
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
typedef enum fate_node_type fate_node_type;

/*
 * How can anonymous nodes be created ?
 *
 * Have the following tree structure :
 *
 * Earth       <= Enode
 * `- Sea      <= Enode
 * `- Ground   <= Enode
 *
 * Attempting to _access_ "Earth.Sky.Bird" will change the tree like so :
 *
 * Earth       <= Enode
 * `- Sky      <= anonymous node
 *    `- Bird  <= anonymous node
 * `- Sea      <= Enode
 * `- Ground   <= Enode
 *
 * Such an access can be intentional or not. 
 * It can also be caused by a dangling SymLink.
 */


/* The following flags apply only to Knodes. */
enum fate_value_size {
    FATE_8 = 0,
    FATE_16 = 1,
    FATE_32 = 2,
    FATE_64 = 3,
};
typedef enum fate_value_size fate_value_size;

enum fate_value_type {
    FATE_UNSIGNED      = 0,
    FATE_SIGNED        = 1,
    FATE_FLOATINGPOINT = 2
};
typedef enum fate_value_type fate_value_type;

enum fate_value_storage {
    FATE_ARRAY  = 0,
    FATE_SINGLE = 1
};
typedef enum fate_value_storage fate_value_storage;

enum fate_owner {
    FATE_OWNED_BY_USER      = 0,
    FATE_OWNED_BY_SCHEDULER = 1
};
typedef enum fate_owner fate_owner;

/* The permissions applies to an individual node. */
enum fate_perms {
    FATE_PERMS_NONE        = 0,
    FATE_PERMS_READ        = 1,
    FATE_PERMS_WRITE       = 2,
    FATE_PERMS_READWRITE   = 3, /* Notice that 3 == 1|2 */
    FATE_PERMS_ADDCHILDREN = 4,
    FATE_PERMS_DELETE      = 8
};
typedef enum fate_perms fate_perms;

/* Should be of 32 bits. */
#define FATE_LEAF_ATTRIBUTES \
    fate_node_type      type           : 4; \
    fate_value_storage  value_storage  : 1; \
    fate_value_type     value_type     : 2; \
    fate_value_size     value_size     : 2; \
    fate_owner          owner          : 1; \
    fate_perms          perms          : 4; \
    unsigned            network_shared : 1; \
    unsigned            reserved       : 9; \
    unsigned char       links_count


#define FATE_NODE_ATTRIBUTES \
    FATE_LEAF_ATTRIBUTES; \
    fate_dentry     *dentries; \
    unsigned     num_dentries

struct fate_symlink {
    FATE_LEAF_ATTRIBUTES;
    uint8_t *target_path;
};
typedef struct fate_symlink fate_symlink;

struct fate_node {
    FATE_NODE_ATTRIBUTES;
};
typedef struct fate_node fate_node;

struct fate_dentry {
    uint8_t   *name;
    fate_node *node;
};
typedef struct fate_dentry fate_dentry;


#endif /* FATE_NODE_H */
