/* Compile-time constants (defined by build process) */

#define FATE_ENABLE_TRACING
#define FATE_DEBUG_BUILD


/* Compile-time constants */

#define XSTRINGIFY(X)
#define STRINGIFY(X)
#define FATE_DEFS_WINDOWS
#define FATE_DEFS_LINUX
#define FATE_DEFS_OSX
#define FATE_DEFS_FREEBSD


/* Global state */

struct fate_globalstate {
    FILE *logf_stream;
    FILE *logf_trace_stream;
    FILE *logf_err_stream;
    FILE *logf_video_stream;
    FILE *logf_audio_stream;
    fate_vfs vfs;
};
typedef struct fate_globalstate fate_globalstate;

/* Things that are done on start */

//fate_gs = &fate_gs_instance;
//fate_globalstate_init(fate_gs);
//fate_globalstate_run(fate_gs, -1); //-1 = Run as long as possible
//fate_globalstate_deinit(fate_gs);
//return 0;
//
// What it does :
//Read command-line args
//Read data/faterc.cfg
//Install signal handler
//Remap input
//Setup VFS
//Initialize root node

/* Logging */

void fate_logf(const char *fmt, ...);
void fate_logf_trace(const char *fmt, ...);
void fate_logf_err(const char *fmt, ...);
void fate_logf_video(const char *fmt, ...);
void fate_logf_audio(const char *fmt, ...);
void fate_log_stacktrace(void (*logfunc)(const char *fmt, ...));


/* Utils API : */

inline void fate_memswap1(uint8_t  *a, uint8_t  *b);
inline void fate_memswap2(uint16_t *a, uint16_t *b);
inline void fate_memswap4(uint32_t *a, uint32_t *b);
inline void fate_memswap8(uint64_t *a, uint64_t *b);
inline void fate_memswapN(void *a, void *b, size_t s);

void fate_memreverse(void *p, uint64_t nbytes);


/* Filesystem API : */

void fate_vfs_init(fate_vfs *fs);
void fate_vfs_make_current(fate_vfs *fs);
void fate_vfs_deinit(fate_vfs *fs);

/*
 * resolveN must return a chunk-allocated array of dentries for efficiency.
 * The dentries in the array are untouched. So there's not information about parents.
 *
 * Therefore, to make a 'ls' command, one will have to call dresolveNat(node, "foo_*", flags), 
 * then print the names of the dentries, and apply the process recursively to the returned nodes.
 * The essentially means manually tokenizing the wanted path. E.g "foo.*_bar.x" -> "foo", "*_bar", "x".
 */

struct fate_nodelist {
    fate_node **nodes;
    unsigned length;
};
void fate_nodelist_dealloc(fate_nodelist *l);

struct fate_dentrylist {
    fate_dentry **dentries;
    unsigned length;
};
void fate_dentrylist_dealloc(fate_dentrylist *l);

/* Node API (requires a current vfs) :
 * flags for resolve*():
 *   FATE_RESOLVE_NO_FOLLOW //If the last item in path is a symlink, return it instead of resolving its target.
 *   FATE_RESOLVE_NO_CACHE_READ //Resolve without looking up the cache.
 *   FATE_RESOLVE_NO_CACHE_WRITE //Don't cache the results.
 *   FATE_RESOLVE_NO_CACHE //Combination of both above.
 */

fate_dentry*     fate_dresolve(fate_node *parent, const uint8_t *path, uint32_t flags);
/* Actually returns fate_dresolve(...)->node. */
fate_node*       fate_resolve(fate_node *parent, const uint8_t *path, uint32_t flags);
/* The resolveN...'s result must be dealloc()'d by the caller. */
fate_dentrylist* fate_dresolveN(fate_node *parent, const uint8_t *path, uint32_t flags);
fate_nodelist*   fate_resolveN(fate_node *parent, const uint8_t *path, uint32_t flags);
uint8_t*         fate_serialize(const fate_node *n, uint32_t flags);
fate_node*       fate_deserialize(const uint8_t *stream, uint32_t flags);
void             fate_create(fate_node *parent, const uint8_t *name, const fate_node *initializer);
void             fate_symlink(fate_node *parent, const uint8_t *name, const uint8_t *target);
const uint8_t*   fate_readlink(const fate_symlink *lnk);
void             fate_link(fate_node *src, fate_node *dst_parent, const uint8_t *dst_name);
void             fate_unlink(fate_node *parent, const uint8_t *name);
void             fate_copy(fate_node *src, fate_node *dst_parent, const uint8_t *dst_name);
void             fate_move(fate_node *src_parent, const uint8_t *src_name, 
                           fate_node *dst_parent, const uint8_t *dst_name);
void             fate_rename(fate_node *n, const uint8_t *new_name);
void             fate_ioctl(fate_node *n, uint32_t op, ...);


/* Engine Event API : */

void                 fate_ee_mask(uint32_t mask);
uint32_t             fate_ee_get_mask(void);
void                 fate_ee_sendto(fate_ee_mod_handle modh, const fate_ee_event *e);
fate_ee_watch_handle fate_ee_watch(const uint8_t *path_no_wilcards, uint32_t mask);
void                 fate_ee_unwatch(fate_ee_watch_handle wh);

