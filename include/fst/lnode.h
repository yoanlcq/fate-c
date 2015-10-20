#ifndef FST_LNODE_H
#define FST_LNODE_H

#include <fst/node.h>

struct fst_lnode {
    FST_NODE_ATTRIBUTES;
    /* stuff */
};
typedef struct fst_lnode fst_lnode;

inline fst_lnode* fst_lnode_alloc(void);
inline       void fst_lnode_dealloc_extras(fst_lnode *ln);

#endif /* FST_LNODE_H */
