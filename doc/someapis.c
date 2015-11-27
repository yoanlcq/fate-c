/* Compile-time constants that are defined by the build process */

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

/* Logging API */

void fate_logf(const char *fmt, ...);
void fate_logf_trace(const char *fmt, ...);
void fate_logf_err(const char *fmt, ...);
void fate_logf_video(const char *fmt, ...);
void fate_logf_audio(const char *fmt, ...);
void fate_log_stacktrace(void (*logfunc)(const char *fmt, ...));


/* Utils API : (Mostly useful for signal processing) */

void      fate_mem_swap1(uint8_t  *a, uint8_t  *b);
void      fate_mem_swap2(uint16_t *a, uint16_t *b);
void      fate_mem_swap4(uint32_t *a, uint32_t *b);
void      fate_mem_swap8(uint64_t *a, uint64_t *b);
void      fate_mem_swapN(void     *a, void     *b, uint64_t nbytes);
void      fate_mem_reverse(void *p, uint64_t nbytes);
/* Convert Mono audio to Multi-channel */
uint8_t*  fate_mem_split1(uint8_t  *p, unsigned num_channels, uint64_t num_frames);
uint16_t* fate_mem_split2(uint16_t *p, unsigned num_channels, uint64_t num_frames);
uint32_t* fate_mem_split4(uint32_t *p, unsigned num_channels, uint64_t num_frames);
uint64_t* fate_mem_split8(uint64_t *p, unsigned num_channels, uint64_t num_frames);
void*     fate_mem_splitN(void     *p, unsigned num_channels, uint64_t num_frames, size_t item_size);
float*    fate_mem_splitf32(float  *p, unsigned num_channels, uint64_t num_frames);
double*   fate_mem_splitf64(double *p, unsigned num_channels, uint64_t num_frames);
/* Convert Multi-channel audio to Mono */
uint8_t*  fate_mem_merge1(uint8_t  *p, unsigned num_channels, uint64_t num_frames);
uint16_t* fate_mem_merge2(uint16_t *p, unsigned num_channels, uint64_t num_frames);
uint32_t* fate_mem_merge4(uint32_t *p, unsigned num_channels, uint64_t num_frames);
uint64_t* fate_mem_merge8(uint64_t *p, unsigned num_channels, uint64_t num_frames);
void*     fate_mem_mergeN(void     *p, unsigned num_channels, uint64_t num_frames, size_t item_size);
float*    fate_mem_mergef32(float  *p, unsigned num_channels, uint64_t num_frames);
double*   fate_mem_mergef64(double *p, unsigned num_channels, uint64_t num_frames);



/* Virtual Filesystem API : 
 * The Node API functions operate on the current VFS.
 * A VFS needs to be init()'ed before it can be made current.
 * A VFS holds :
 * -> The memory pools which contain the nodes;
 * -> Zero or more caches, for faster path resolution;
 * -> Optionally, a journal, that keeps tracks of changes 
 *    in the VFS, and provides an "Undo/redo" functionality.
 *
 * Features that could be provided :
 * -> Copy-on-Write (or Lazy writing)
 * -> Node-level snapshots (queuing read/writes while saving the node (also called freezing));
 * -> Checksums (for network gaming);
 * -> Defragmentation (for long-term persistent worlds).
 * -> Encryption
 * -> Eager allocation
 * -> Lazy allocation
 */

void fate_vfs_init(fate_vfs *fs);
void fate_vfs_deinit(fate_vfs *fs);
void fate_vfs_make_current(fate_vfs *fs);

/* The following functions operate on the current VFS, because they are exposed to its users. */

/* cache_flags may be 0 or a bitwise-OR combination of :
 *  FATE_CACHE_READ  //Read the cache while resolving paths.
 *  FATE_CACHE_WRITE //Write path resolution results to the cache.
 * The point of this function is to be called at any moment.
 * Before creating a node, if we know that no node exists with that name, we should disable cache read.
 * Similarly, before creating a node that is not likely to be accessed, we should disable cache write.
 * In both case, the cache flags should be restored after we're done.
 */
void fate_vfs_cache_flags(uint32_t flags);
uint32_t fate_vfs_get_cache_flags(void);

/*
 * fate_glob() must return a chunk-allocated array of dentries for efficiency.
 * The dentries in the array are untouched. So there's no information about parents.
 *
 * Therefore, to make a 'ls' command, one will have to call fate_dglob(node, "foo_*"), 
 * then print the names of the dentries, and apply the process recursively to the returned nodes.
 * The essentially means manually tokenizing the wanted path. E.g "foo.*_bar.x" -> "foo", "*_bar", "x".
 */

/* Node API (requires a current vfs) :
 *
 * To optimize caching, the caller should use fate_vfs_cache_flags() before and after resolve().. etc.
 * When we want to prevent operations from overwriting exisiting nodes, we just check for the node's
 * existence with fate_resolve() and then make decisions.
 * All operations overwrite existing nodes when they exist.
 * Please don't make assumptions based on their similarities with OS-specific file APIs/utilities.
 *
 * serialization formats for (de)serialize:
 *   FATE_SERIALIZATION_JSON
 *   FATE_SERIALIZATION_XML
 *   FATE_SERIALIZATION_BIN
 * compression formats for (de)serialize:
 *   FATE_COMPRESSION_NONE = 0
 *   FATE_COMPRESSION_ZIP
 *   FATE_COMPRESSION_GZIP
 *   FATE_COMPRESSION_BZIP2
 */
/* 
 * fate_dresolve() returns NULL if no node matches the path. 
 * 'parent' may be NULL. In that case, path resolution is relative to the VFS's root.
 * If 'parent' is NULL, then 'path' may be NULL or empty :
 *     In that case, the VFS's root's dentry is returned.
 */
fate_dentry*   fate_dresolve(fate_node *parent, const uint8_t *path);
/* 
 * fate_resolve() is a convenience function, because what it actually does is :
 * fate_dentry *dentry = fate_dresolve(parent, path);
 * return dentry ? dentry->node : NULL;
 */
fate_node*     fate_resolve(fate_node *parent, const uint8_t *path);
/* These variants do not dereference the last item in the path if it is a symbolic link. */
fate_dentry*   fate_dresolve_nofollow(fate_node *parent, const uint8_t *path);
fate_node*     fate_resolve_nofollow(fate_node *parent, const uint8_t *path);
/* The glob's result is a NULL-terminated array of pointers which must be free()'d by the caller. */
fate_dentry**  fate_dglob(fate_node *parent, const uint8_t *path_pattern);
fate_node**    fate_glob(fate_node *parent, const uint8_t *path_pattern);
/* (de)serialize's result must be free()'s by the caller. */
const uint8_t* fate_serialize(const fate_node *n, fate_serialization_format fmt, fate_compression_format c);
/* The compression and serialization formats are determined with magic numbers. 
 * 'data' _MUST_ be NULL-terminated. */
fate_node*     fate_deserialize(const uint8_t *data);
void           fate_link(fate_node *src, fate_node *dst_parent, const uint8_t *dst_name);
void           fate_unlink(fate_node *parent, const uint8_t *name);
/*
 * fate_create() is a convenience function, because it is equivalent to using fate_link() 
 * with 'src' having a links count of 0.
 * It is the preferred way of adding nodes to the VFS because it is journal-friendly.
 * Do not use it to create symbolic links, use fate_symlink() instead.
 */
void           fate_create(const fate_node *src, fate_node *parent, const uint8_t *name);
void           fate_symlink(fate_node *parent, const uint8_t *name, const uint8_t *target);
/* This one really is just a convenience function. It returns NULL if the node is not a symlink. */
const uint8_t* fate_readlink(const fate_node *lnk);
/* It's different from link() and create() because it recursively copies the children with it. 
 * Also, the source node is assumed to exist in the VFS (not from anywhere else !). */
void           fate_copy(const fate_node *src, fate_node *dst_parent, const uint8_t *dst_name);
/*
 * fate_move() is a convenience function, because it is equivalent to :
 * fate_node *src = fate_resolve(src_parent, src_name);
 * if(src) {
 *     fate_link(src, dst_parent, dst_name);
 *     fate_unlink(src_parent, src_name);
 * } else fate_logf_err("Warning : blablah\n");
 * However, it is the preferred way of moving nodes though the VFS, because it is journal-friendly.
 */
void           fate_move(fate_node *src_parent, const uint8_t *src_name, 
                         fate_node *dst_parent, const uint8_t *dst_name);
/* 'new_name' must be a name, not a path. If a sibling is already named 'new_name', 
 * it is unlinked before renaming occurs. 
 * It differs from fate_move() in the sense that instead of using fate_link() and fate_unlink(),
 * the dentry's name is just freed and replaced by a dynamic copy of 'new_name'.
 * It means that theoretically, it should perform faster.
 * 'parent''s sole purpose is to check the dentry's siblings for collisions.
 */
void           fate_rename(fate_node *parent, fate_dentry *d, const uint8_t *new_name);
/* 
 * A catch-all function similar to Linux's ioctl() system call. 
 * It may prove useful to mods, or internal work-in-progress features.
 */
void           fate_node_op(fate_node *n, uint32_t op, ...);


/* Engine Event API : */

void                 fate_ee_mask(uint32_t mask);
uint32_t             fate_ee_get_mask(void);
void                 fate_ee_sendto(fate_ee_mod_handle modh, const fate_ee_event *e);
fate_ee_watch_handle fate_ee_watch(const uint8_t *path_no_wilcards, uint32_t mask);
void                 fate_ee_unwatch(fate_ee_watch_handle wh);

