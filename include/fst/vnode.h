#ifndef FST_VNODE_H
#define FST_VNODE_H

#include <fst/node.h>

struct fst_vnode {
    FST_NODE_ATTRIBUTES;
    /* extras, such as viewport, GUI plane, etc */
};
typedef struct fst_vnode fst_vnode;

inline fst_vnode* fst_vnode_alloc(void);
inline void fst_vnode_dealloc_extras(fst_vnode *vn);

#endif /* FST_LNODE_H */
