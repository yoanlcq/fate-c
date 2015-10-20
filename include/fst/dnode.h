#ifndef FST_DNODE_H
#define FST_DNODE_H

#include <fst/node.h>

struct fst_dnode {
    FST_NODE_ATTRIBUTES;
    /* extras */
};
typedef struct fst_dnode fst_dnode;

inline fst_dnode* fst_dnode_alloc (void);
inline void fst_dnode_dealloc_extras(fst_dnode *dn);

#endif /* FST_LNODE_H */
