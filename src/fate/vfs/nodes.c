#include <fst/config.h>
#include <fst/nodes.h>
#include <fst/node.h>
#include <fst/lnode.h>
#include <fst/knode.h>
#include <fst/dnode.h>
#include <fst/enode.h>
#include <fst/vnode.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>

void fst_anynode_printl(FILE *stream, fst_node *n) {
    fprintf(stream, "%s ", n->name);
    if(n->entry_type == FST_SYMLINK) {
        fprintf(stream, "=> %s (", ((fst_symlink*)n)->target_path);
    } else {
        if(n->type==FST_KNODE) {
            uint32_t i, *str32;
            switch(n->value_type) {
                case FST_KNODE_STR32: 
                    str32 = ((fst_knode_str32*)n)->value;
                    fprintf(stream, "= \"");
                    for(i=0 ; str32[i] ; ++i)
                        fputwc(str32[i], stream);
                    fputc(stream, "\"");
                    break;
                case FST_KNODE_STR8: fprintf(stream, "= \"%s\"",((fst_knode_str8*)n)->value); break;
                case FST_KNODE_S8:   fprintf(stream, "= %hhd",  ((fst_knode_s8  *)n)->value); break;
                case FST_KNODE_S16:  fprintf(stream, "= %hd",   ((fst_knode_s16 *)n)->value); break;
                case FST_KNODE_S32:  fprintf(stream, "= %d",    ((fst_knode_s32 *)n)->value); break;
                case FST_KNODE_S64:  fprintf(stream, "= %ld",   ((fst_knode_s64 *)n)->value); break;
                case FST_KNODE_U8:   fprintf(stream, "= %hhu",  ((fst_knode_u8  *)n)->value); break;
                case FST_KNODE_U16:  fprintf(stream, "= %hu",   ((fst_knode_u16 *)n)->value); break;
                case FST_KNODE_S32:  fprintf(stream, "= %u",    ((fst_knode_u32 *)n)->value); break;
                case FST_KNODE_U64:  fprintf(stream, "= %lu",   ((fst_knode_u64 *)n)->value); break;
                case FST_KNODE_F32:  fprintf(stream, "= %f",    ((fst_knode_f32 *)n)->value); break;
                case FST_KNODE_F64:  fprintf(stream, "= %lf",   ((fst_knode_f64 *)n)->value); break;
            }
            fprintf(stream, " (%s, ", fst_value_type_names[n->value_type][0]);
        } else {
            fprintf(stream, " (%s, ", fst_node_type_names[n->type][0]);
        }
    }

    fprintf(stream, "access:%u%s", n->access, 
                    n->role=FST_ROLE_SCHEDULER ? " (Sched)" : "");
    if(n->links_count != 1) /* We want to know if it is 0, in which case we should worry. */
        fprintf(stream, " (%u links)", n->links_count);
    fprintf(stream, ")\n");

}

void fst_anynode_dealloc(fst_node *n, uint8_t depth) {
    switch(n->type) {
        /* Nothing here for typeless node. We just free its name and itself and the end. */
        case FST_LNODE: fst_lnode_dealloc_extras(n); break;
        case FST_KNODE: fst_knode_dealloc_extras(n); break;
        case FST_DNODE: fst_dnode_dealloc_extras(n); break;
        case FST_ENODE: fst_enode_dealloc_extras(n); break;
        case FST_VNODE: fst_vnode_dealloc_extras(n); break;
    }
#define CNT (((fst_hardlink*)n)->children.count)
#define CHILD(__i__) ((fst_hardlink*)n)->children.nodes[__i__]
    if(depth && n->entry_type == FST_HARDLINK && CNT > 0) {
        for(--CNT ; ; --CNT) {
            fst_anynode_dealloc(CHILD(CNT), depth-1);
            if(!CNT) break;
        }
    }
#undef CNT
#undef CHILD
    free(n->name);
    free(n);
}

void fst_nodestack_dump(FILE *stream, fst_nodestack *s, unsigned from_top, unsigned from_bot) {
    fst_node *n;

    fprintf(stream, "Stack dump (from top to bottom) :\n");
    if(s->count <= from_top + from_bot) {
        for(;;) {
            n = fst_nodestack_pop(s);
            if(!n) return;
            fst_anynode_printl(stream, n);
        }
    }
    unsigned i;
    for(i=0 ; i<from_top ; ++i)
        fst_anynode_printl(stream, fst_nodestack_pop(s));
    fprintf(stream, "\n... (Skipping %u node%s)\n\n", 
            s->count-from_top-from_bot, 
            s->count-from_top-from_bot==1 ? "" : "s");
    for(i=0 ; i<s->count-from_top-from_bot ; ++i)
        fst_nodestack_pop(s);
    for(;;) {
        n = fst_nodestack_pop(s);
        if(!n) return;
        fst_anynode_printl(stream, n);
    }
}

static void fst_nodestack_handle_nomem(fst_nodestack *s, unsigned requested_count) {
    fprintf(stderr, "Failed to allocate memory for a node stack !\n"
                    "(Each node weights %u bytes)\n"
                    "(Current capacity   : %u nodes)\n"
                    "(Requested capacity : %u nodes)\n"
                    "\nPossible causes are :\n"
                    "-> A node has requested too many children;"
                    "-> An infinite node loop;"
                    "-> There is simply not enough memory available.\n\n",
                    sizeof(fst_node*), s->count, requested_count);
    fst_nodestack_dump(stderr, s, 128, 128);
    fst_abort();
}

static void fst_nodestack_handle_overflow(fst_nodestack *s) {
    fprintf(stderr, "Detected integer overflow on a node stack's size.\n"
                    "This may be caused by an undetected infinite node loop.\n");
    fst_nodestack_dump(stderr, s, 128, 128);
    fst_abort();
}


/* TODO Justify everthing */
/* TODO Horizontally : Allocate by factors of 2 (until 2^FST_CFG_NODEPACK_CAPACITY_INTSIZE)
 *      Vertically   : Static, until (FST_GLOBALTREE_DEPTH) (count is uint_fast32_t)*/

void fst_nodestack_push (fst_nodestack *s, fst_node *n) {
    fst_node **newnodes;
#define MAKEROOM(__req__) \
    newnodes = realloc(s->nodes, __req__*sizeof(fst_node*)); \
    if(newnodes) \
        s->nodes = newnodes; \
    else fst_nodestack_handle_nomem(s, __req__);
 
    switch(s->count) {
        case 0x00000000U          : s->nodes = NULL;
                                    MAKEROOM(0x00000001U          ); break;
        case 0x00000001U          : MAKEROOM(0x00000002U          ); break;
        case 0x00000002U          : MAKEROOM(0x00000004U          ); break;
        case 0x00000004U          : MAKEROOM(0x00000008U          ); break;
        case 0x00000008U          : MAKEROOM(0x00000010U          ); break;
        case 0x00000010U          : MAKEROOM(0x00000020U          ); break;
        case 0x00000020U          : MAKEROOM(0x00000040U          ); break;
        case 0x00000040U          : MAKEROOM(0x00000080U          ); break;
#if FST_CFG_NODEPACK_CAPACITY_INTSIZE == 8
        case 0x00000080U          : MAKEROOM(0x000000ffU          ); break;
        case 0x000000ffU: fst_nodestack_handle_overflow(s); break;
#else
        case 0x00000080U          : MAKEROOM(0x00000100U          ); break;
#endif
#if FST_CFG_NODEPACK_CAPACITY_INTSIZE >= 16
        case 0x00000100U          : MAKEROOM(0x00000200U          ); break;
        case 0x00000200U          : MAKEROOM(0x00000400U          ); break;
        case 0x00000400U          : MAKEROOM(0x00000800U          ); break;
        case 0x00000800U          : MAKEROOM(0x00001000U          ); break;
        case 0x00001000U          : MAKEROOM(0x00002000U          ); break;
        case 0x00002000U          : MAKEROOM(0x00004000U          ); break;
        case 0x00004000U          : MAKEROOM(0x00008000U          ); break;
#if FST_CFG_NODEPACK_CAPACITY_INTSIZE == 16
        case 0x00008000U          : MAKEROOM(0x0000ffffU          ); break;
        case 0x0000ffffU: fst_nodestack_handle_overflow(s); break;
#else
        case 0x00008000U          : MAKEROOM(0x00010000U          ); break;
#endif
#if FST_CFG_NODEPACK_CAPACITY_INTSIZE >= 32
        case 0x00010000U          : MAKEROOM(0x00020000U          ); break;
        case 0x00020000U          : MAKEROOM(0x00040000U          ); break;
        case 0x00040000U          : MAKEROOM(0x00080000U          ); break;
        case 0x00080000U          : MAKEROOM(0x00100000U          ); break;
        case 0x00100000U          : MAKEROOM(0x00200000U          ); break;
        case 0x00200000U          : MAKEROOM(0x00400000U          ); break;
        case 0x00400000U          : MAKEROOM(0x00800000U          ); break;
        case 0x00800000U          : MAKEROOM(0x01000000U          ); break;
        case 0x01000000U          : MAKEROOM(0x02000000U          ); break;
        case 0x02000000U          : MAKEROOM(0x04000000U          ); break;
        case 0x04000000U          : MAKEROOM(0x08000000U          ); break;
        case 0x08000000U          : MAKEROOM(0x10000000U          ); break;
        case 0x10000000U          : MAKEROOM(0x20000000U          ); break;
        case 0x20000000U          : MAKEROOM(0x40000000U          ); break;
        case 0x40000000U          : MAKEROOM(0x80000000U          ); break;
#if FST_CFG_NODEPACK_CAPACITY_INTSIZE == 32
        case 0x80000000U          : MAKEROOM(0xffffffffU          ); break;
        case 0xffffffffU: fst_nodestack_handle_overflow(s); break;
#else
        case 0x80000000U          : MAKEROOM(0x0000000100000000ULL); break;
#endif
#if FST_CFG_NODEPACK_CAPACITY_INTSIZE >= 64
        case 0x0000000100000000ULL: MAKEROOM(0x0000000200000000ULL); break;
        case 0x0000000200000000ULL: MAKEROOM(0x0000000400000000ULL); break;
        case 0x0000000400000000ULL: MAKEROOM(0x0000000800000000ULL); break;
        case 0x0000000800000000ULL: MAKEROOM(0x0000001000000000ULL); break;
        case 0x0000001000000000ULL: MAKEROOM(0x0000002000000000ULL); break;
        case 0x0000002000000000ULL: MAKEROOM(0x0000004000000000ULL); break;
        case 0x0000004000000000ULL: MAKEROOM(0x0000008000000000ULL); break;
        case 0x0000008000000000ULL: MAKEROOM(0x0000010000000000ULL); break;
        case 0x0000010000000000ULL: MAKEROOM(0x0000020000000000ULL); break;
        case 0x0000020000000000ULL: MAKEROOM(0x0000040000000000ULL); break;
        case 0x0000040000000000ULL: MAKEROOM(0x0000080000000000ULL); break;
        case 0x0000080000000000ULL: MAKEROOM(0x0000100000000000ULL); break;
        case 0x0000100000000000ULL: MAKEROOM(0x0000200000000000ULL); break;
        case 0x0000200000000000ULL: MAKEROOM(0x0000400000000000ULL); break;
        case 0x0000400000000000ULL: MAKEROOM(0x0000800000000000ULL); break;
        case 0x0000800000000000ULL: MAKEROOM(0x0001000000000000ULL); break;
        case 0x0001000000000000ULL: MAKEROOM(0x0002000000000000ULL); break;
        case 0x0002000000000000ULL: MAKEROOM(0x0004000000000000ULL); break;
        case 0x0004000000000000ULL: MAKEROOM(0x0008000000000000ULL); break;
        case 0x0008000000000000ULL: MAKEROOM(0x0010000000000000ULL); break;
        case 0x0010000000000000ULL: MAKEROOM(0x0020000000000000ULL); break;
        case 0x0020000000000000ULL: MAKEROOM(0x0040000000000000ULL); break;
        case 0x0040000000000000ULL: MAKEROOM(0x0080000000000000ULL); break;
        case 0x0080000000000000ULL: MAKEROOM(0x0100000000000000ULL); break;
        case 0x0100000000000000ULL: MAKEROOM(0x0200000000000000ULL); break;
        case 0x0200000000000000ULL: MAKEROOM(0x0400000000000000ULL); break;
        case 0x0400000000000000ULL: MAKEROOM(0x0800000000000000ULL); break;
        case 0x0800000000000000ULL: MAKEROOM(0x1000000000000000ULL); break;
        case 0x1000000000000000ULL: MAKEROOM(0x2000000000000000ULL); break;
        case 0x2000000000000000ULL: MAKEROOM(0x4000000000000000ULL); break;
        case 0x4000000000000000ULL: MAKEROOM(0x8000000000000000ULL); break;
        case 0x8000000000000000ULL: MAKEROOM(0xffffffffffffffffULL); break;
        case 0xffffffffffffffffULL: fst_nodestack_handle_overflow(s); break;
    }
    s->nodes[s->count] = n;
    ++(s->count);
#endif /* 64 */
#endif /* 32 */
#endif /* 16 */
#undef MAKEROOM
}

inline void fst_nodestack_static_push(fst_nodestack *s, fst_node *n) {
    if(s->count == FST_CFG_MAX_PATH_STACK)
        fst_nodestack_handle_overflow(s);
    s->nodes[s->count] = n;
    ++(s->count);
}

/* FIXME !! */
inline fst_node *fst_nodestack_pop (fst_nodestack *s) {
    if(!s->count)
        return NULL;
    --(s->count);
    fst_node *res = s->nodes[s->count];
    switch(s->count) {
        case 0x00: free(s->nodes); break;
        case 0x04: newnodes = realloc(s->nodes, 0x04*sizeof(fst_node*)); newnodes ? s->nodes = newnodes : fst_nodestack_handle_nomem(s, 0x04); break;
        case 0x08: newnodes = realloc(s->nodes, 0x08*sizeof(fst_node*)); newnodes ? s->nodes = newnodes : fst_nodestack_handle_nomem(s, 0x08); break;
        case 0x10: newnodes = realloc(s->nodes, 0x10*sizeof(fst_node*)); newnodes ? s->nodes = newnodes : fst_nodestack_handle_nomem(s, 0x10); break;
        case 0x20: newnodes = realloc(s->nodes, 0x20*sizeof(fst_node*)); newnodes ? s->nodes = newnodes : fst_nodestack_handle_nomem(s, 0x20); break;
        case 0x40: newnodes = realloc(s->nodes, 0x40*sizeof(fst_node*)); newnodes ? s->nodes = newnodes : fst_nodestack_handle_nomem(s, 0x40); break;
        case 0x80: newnodes = realloc(s->nodes, 0x80*sizeof(fst_node*)); newnodes ? s->nodes = newnodes : fst_nodestack_handle_nomem(s, 0x80); break;
    }
    return res;
}

inline fst_node *fst_nodestack_static_pop(fst_nodestack *s) {
    if(!s->count)
        return NULL;
    --(s->count);
    return s->nodes[s->count];
}

inline fst_node *fst_nodestack_top (fst_nodestack *s) {
    return s->count ? s->nodes[s->count-1] : NULL;
}

inline void fst_nodestack_clear (fst_nodestack *s) {
    if(s->count)
        free(s->nodes);
    s->nodes = NULL;
    s->count = 0;
}

