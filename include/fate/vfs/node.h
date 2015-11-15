#ifndef FST_NODE_H
#define FST_NODE_H

#include <stdint.h>

enum fst_node_type {
    FST_NODE  = 0, /* For typeless nodes. See below. */
    FST_LNODE = 1,
    FST_KNODE = 2,
    FST_DNODE = 3,
    FST_ENODE = 4,
    FST_VNODE = 5,
    FST_SNODE = 6, /* State nodes are wrapper for function pointers. */
};
typedef enum fst_node_type fst_node_type;
const char *fst_node_type_names[7][2] = {
    {"Node",  "Typeless Node"}, 
    {"Lnode", "Local Device Node"},
    {"Knode", "Value Node"},
    {"Dnode", "Entity Data Node"},
    {"Enode", "Entity Instance Node"},
    {"Vnode", "View Node"},
    {"Snode", "State Node"}
};

/*
 * How can typeless nodes be created ?
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
 * `- Sky      <= Typeless node
 *    `- Bird  <= Typeless node
 * `- Sea      <= Enode
 * `- Ground   <= Enode
 *
 * Such an access can be intentional or not. 
 * It can also be caused by a dangling SymLink.
 */


/* The following flags apply only to Knodes. */
enum fst_value_type {
    FST_KNODE_NONE  =  0, /* Unused and actually only for debug printing. */
    FST_KNODE_UTF8  =  1,
    FST_KNODE_S8    =  2,
    FST_KNODE_S16   =  3,
    FST_KNODE_S32   =  4,
    FST_KNODE_S64   =  5,
    FST_KNODE_U8    =  6,
    FST_KNODE_U16   =  7,
    FST_KNODE_U32   =  8,
    FST_KNODE_U64   =  9,
    FST_KNODE_F32   = 10,
    FST_KNODE_F64   = 11
};
typedef enum fst_value_type fst_value_type;
const char *fst_value_type_names[13][2] = {
    {"None",  "No value type"},
    {"utf8",  "UTF-8 string"},
    {"s8",    "Signed 8-bit integer"},
    {"s16",   "Signed 16-bit integer"},
    {"s32",   "Signed 32-bit integer"},
    {"s64",   "Signed 64-bit integer"},
    {"u8",    "Unsigned 8-bit integer"},
    {"u16",   "Unsigned 16-bit integer"},
    {"u32",   "Unsigned 32-bit integer"},
    {"u64",   "Unsigned 64-bit integer"},
    {"f32",   "Single-precision floating-point number"},
    {"f64",   "Double-precision floating-point number"}
};

enum fst_entry_type {
    FST_HARDLINK = 0,
    FST_SYMLINK  = 1
};
typedef enum fst_entry_type fst_entry_type;
const char *fst_entry_type_names[2] = {
    "HardLink", "SymLink"
};

enum fst_owner {
    FST_OWNED_BY_USER      = 0,
    FST_OWNED_BY_SCHEDULER = 1
};
typedef enum fst_owner fst_owner;
const char *fst_owner_names[2] = {
    "User", "Scheduler"
};

/* The permissions applies to an individual node. */
enum fst_perms {
    FST_PERMS_NONE        = 0,
    FST_PERMS_READ        = 1,
    FST_PERMS_WRITE       = 2,
    FST_PERMS_READWRITE   = 3, /* Notice that 3 == 1|2 */
    FST_PERMS_ADDCHILDREN = 4,
    FST_PERMS_DELETE      = 8
};
typedef enum fst_perms fst_perms;

#define FST_LEAF_ATTRIBUTES \
    fst_entry_type  entry_type  : 1; \
    fst_node_type   type        : 3; \
    fst_value_type  value_type  : 4; \
    fst_owner       owner       : 1; \
    fst_perms       perms       : 4; \
    unsigned        network_shared : 1; \
    unsigned        reserved    : 2; \
    unsigned char   links_count

#define FST_NODE_ATTRIBUTES \
    FST_LEAF_ATTRIBUTES; \
    fst_dentry     *dentries

struct fst_node {
    FST_NODE_ATTRIBUTES;
};
typedef struct fst_node fst_node;

struct fst_dentry {
    uint8_t  *name;
    fst_node *node;
};
typedef struct fst_dentry fst_dentry;

struct fst_symlink {
    FST_LEAF_ATTRIBUTES;
    uint8_t *target_path;
};
typedef struct fst_symlink fst_symlink;

#endif /* FST_NODE_H */
