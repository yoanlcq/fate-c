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
