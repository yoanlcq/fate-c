#include <fst/macros.h>
#include <fst/globaltree.h>
#include <fst/node.h>
#include <fst/lnode.h>
#include <fst/knode.h>
#include <fst/dnode.h>
#include <fst/enode.h>
#include <fst/vnode.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

fst_globaltree *fst_used_globaltree;

fst_node* fst_cwnstack_staticbuf[255];
fst_nodestack fst_cwnstack = {fst_cwnstack_staticbuf, 0};
fst_node* fst_rootstack_staticbuf[255];
fst_nodestack fst_rootstack = {fst_rootstack_staticbuf, 0};


inline void fst_globaltree_init(fst_globaltree *g) {
    g->root = fst_lnode_alloc();
    fst_nodestack_push(&FST_CWN_STACK, g->root);
}
inline void fst_globaltree_deinit(fst_globaltree *g) {
    fst_anynode_dealloc(g->root, 0xff);
    fst_nodestack_clear(&FST_CWN_STACK);
}
inline void fst_globaltree_use(fst_globaltree *g) {
    FST_UGT = g;
    fst_nodestack_static_clear(&fst_rootstack);
    fst_nodestack_static_push(&fst_rootstack, g->root);
}


/* 
 * The function below, taken from :
 * http://www.codeproject.com/Articles/1088/Wildcard-string-compare-globbing 
 * was written by Jack Handy (jakkhandy@hotmail.com).
 * No license was attached.
 * */
static int wildcmp(const char *wild, const char *string) {
  const char *cp = NULL, *mp = NULL;

  while ((*string) && (*wild != '*')) {
    if ((*wild != *string) && (*wild != '?')) {
      return 0;
    }
    wild++;
    string++;
  }

  while (*string) {
    if (*wild == '*') {
      if (!*++wild) {
        return 1;
      }
      mp = wild;
      cp = string+1;
    } else if ((*wild == *string) || (*wild == '?')) {
      wild++;
      string++;
    } else {
      wild = mp;
      string = cp++;
    }
  }

  while (*wild == '*') {
    wild++;
  }
  return !*wild;
}

/* Might be extended later */
#define fst_name_matches(name, expr) wildcmp(expr, name);
#define haswild(name) (strchr(name, '*') || strchr(name, '?'))


bool fst_checkpath(const uint8_t *path) {

    if(path[0] == '\0')
        return true; /* After all, it theoretically resolves to the current working node. */

    bool res = true;

    if(path[0]=='.') {
        fprintf(stderr, "In \"%s\" :\n    ^\nMissing name before first '.'\n", path);
        res = false;
    }

    if(path[0] == '%'
    || path[0] == ':'
    || path[0] == '&'
    || path[0] == '@'
    || path[0] == '~') 
    {
        if(path[1] != '.') {
            fprintf(stderr, "In \"%s\" :\n     ^\nExpected '.' after '%c'\n", path, path[0]);
            res = false;
        }
    } else if(!isalnum(path[0]) && path[0] != '_') {
        fprintf(stderr, "In \"%s\" :\n    ^\nFirst character is illegal\n", path);
        res = false;
    }

    unsigned i, namelen = 0;
    for(i=1 ; path[i] ; ++i)
    {
        if(!isalnum(path[i]) && path[i] != '_') {
            fprintf(stderr, "In \"%s\" :\n%*c^\nIllegal character at %u\n", path, 4+i, i);
            res = false;
        } else if(path[i-1]=='*' && path[i]=='*' && path[i+1] != '*') {
            fprintf(stderr, "In \"%s\" :\n%*c^\nUseless extra '*' operators (before %u)\n", path, 4+i, i);
            res = false;
        } else if(path[i-1]=='.' && path[i]=='.' && path[i+1] != '.') {
            fprintf(stderr, "In \"%s\" :\n%*c^\nUseless extra '.' operators (before %u)\n", path, 4+i, i);
            res = false;
        }
        if(path[i]=='.') {
            if(namelen > FST_NODE_NAME_MAX_LENGTH) {
                fprintf(stderr, "In \"%s\" :\n%*c^\nName is longer than %u characters (at %u)\n", path, 4+i-namelen, i-namelen);
                res = false;
            }
            namelen = 0;
        } else ++namelen;
    }

    --i;
    if(path[i]=='.') {
        fprintf(stderr, "In \"%s\" :\n%*c^\nMissing name after '.' at %u\n", path, 4+i, i);
        res = false;
    }

    return res;
}



/* fst_coreget() returns a collection of nodes by resolving _path_.
 * Being one of the core functions of the engine, and potentially called an insane 
 * amount of time, it must perform crazy fast.
 * The documentation below targets potential maintainers.
 *
 * FIXME FST_CWN_STACK must be filled with the absolute path to CWN 
 * TODO Detect infinite loops and redudancy.
 * FIXME make different versions with compile-time macros.
 * FIXME avoid the recursive approach.
 *
 * Parameters :
 *
 * _path_ is assumed to be a "pure" path (i.e it is not ill-formed, has no redundancy, 
 * and special symbols are translated).
 * Purity of a path can be checked by a call to fst_purify_path().
 *
 * _pathstack_ must contain a stack representing the absolute full path to the 
 * node which _path_ is relative to.
 * For instance, given a tree like "%.Earth.Sea.Me", we could have _path_ be "Sea.Me" 
 * and _pathstack_ hold the "%" and "Earth" nodes.
 * The philosophy behind _pathstack_ is to save space (i.e nodes don't have to store their parent).
 *
 * _resptr_ must be NULL. It is used only for recursive calls, in which case it must hold the 
 * address of the collection of the resolved nodes.
 *
 * _follow_symlinks_ indicates whether Symbolic Links should be followed or not. 
 *
 * _multiple_ indicates whether the user expects _path_ to resolve to several nodes or not.
 * Since the user targets single nodes very often, we must prevent stack operations 
 * like push() or pop() to occur when that's the case, since they imply malloc() and free() overhead.
 * Instead, we'd like to stop at the first node we successfully resolve, and simply return it 
 * (using a fake stack).
 *
 * */

fst_node* fst_coreget_local_pathstack_staticbuf[255];

static fst_nodestack fst_coreget(const uint8_t *path, 
                                 fst_nodestack *pathstack, 
                                 fst_nodestack *resptr,
                                 bool follow_symlinks,
                                 bool multiple)
{
    fst_nodestack local_pathstack = {fst_coreget_local_pathstack_staticbuf, 0};
    fst_nodestack local_res = {NULL, 0};
    /* stk is the pointer to the path stack we really use. 
     * It may be either pathstack or local_pathstack. */
    fst_nodestack *stk;
    /* The same idea applies here. res => result. */
    fst_nodestack *res = resptr ? resptr : &local_res;


    /* If the path is absolute, then we use local_pathstack
     * because it is empty and statically allocated.
     * Otherwise, we use pathstack. */

    if(path[0] == '%') {
        stk = &local_pathstack;
        fst_nodestack_static_push(stk, FST_UGT->root);
        path += 2; /* Skip '%' and '.' */
    } else stk = pathstack;


    uint8_t *ind, name[FST_NODE_NAME_MAX_LENGTH+1];
    for(;;)
    {
        if(*path == '^') 
        {
            /* Go top-level, but not farther above than the root. */
            if(fst_nodestack_top(stk)->type != FST_LNODE)
                fst_nodestack_pop(stk);
            ++path; /* Skip '^' */
            if(*path == '\0')
                break;
            ++path; /* It's '.', skip it. */
            continue;
        }

#ifdef _GNU_SOURCE
        ind = strchrnul(path, '.');
#else
        ind = strchr(path, '.');
        if(!ind) ind = strrchr(path, '\0');
#endif
#ifdef FST_CFG_CHECK_NODE_NAME_OVERFLOW
        if(ind-path > FST_CFG_NODE_NAME_MAX_LENGTH) {
            fprintf("Warning : The name '%s' "
                    "violates the implementation's limit of %u characters.\n", 
                    name, FST_CFG_NODE_NAME_MAX_LENGTH);
            strncpy(name, path, FST_CFG_NODE_NAME_MAX_LENGTH);
            name[FST_CFG_NODE_NAME_MAX_LENGTH] = '\0';
        } else {
#endif
            strncpy(name, path, ind-path);
            name[ind-path] = '\0';
#ifdef FST_CFG_CHECK_FOR_NAME_OVERFLOW
        }
#endif

        unsigned i;
        bool found = false;
        fst_nodestack single_result;
        fst_hardlink *cur = fst_nodestack_top(stk);
        for(i=0 ; i<cur->children.count ; ++i) 
        {
            if(!fst_name_matches(name, cur->children.nodes[i]->name))
                continue;

            found = true;
            if(cur->children.nodes[i]->entry_type == FST_HARDLINK) {
                if(*ind == '\0') { /* Last in path ? */
                    if(multiple)
                        fst_nodestack_push(res, cur->children.nodes[i]);
                    else {
                        single_result.nodes = &(cur->children.nodes[i]); 
                        return single_result;
                    }
                } else {
                    fst_nodestack_static_push(stk, child);
                    fst_coreget(ind+1, stk, res);
                    fst_nodestack_static_pop(stk);
                }
            } else {
                if(follow_symlinks) {
                    /* TODO Store the SymLinks in a special stack to catch infinite loops. */
                    fst_coreget(((fst_symlink*)(cur->children.nodes[i]))->target_path, &FST_CWN_STACK, res);
                }
                else {
                    if(multiple)
                        fst_nodestack_push(res, cur->children.nodes[i]);
                    else {
                        single_result.nodes = &(cur->children.nodes[i]); 
                        return single_result;
                    }
                }
            }
        }

        if(!found && !haswild(name))
            return NULL;
        /* End of path ? */
        if(*ind == '\0')
            break;
    }

    if(multiple && !resptr) {
        /* TODO remove duplicates from res (Hardlink redundancy) */
    }

    return res;
}

/* They determine whether to use the cwn stack or an empty stack. */
fst_nodequeue    fst_res_multiple          (const uint8_t *path) { /* TODO resolve multiple */ }
fst_nodequeue    fst_res_multiple_nofollow (const uint8_t *path) { /* TODO resolve multiple nofollow */ }
fst_symlink*     fst_res_single_nofollow   (const uint8_t *path) { /* TODO resolve single nofollow */ }
void*            fst_res_single            (const uint8_t *path) { /* TODO resolve single */ }

