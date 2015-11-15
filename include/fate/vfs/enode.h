#ifndef FST_ENODE_H
#define FST_ENODE_H

#include <fst/node.h>

struct fst_enode {
    FST_NODE_ATTRIBUTES;
    /* extras, such as resolve cache, VAOs, etc. */
};
typedef struct fst_enode fst_enode;

inline fst_enode* fst_enode_alloc (void);
inline void fst_enode_dealloc_extras(fst_enode *dn);


#endif /* FST_LNODE_H */
