
/* Compile-time constants that are defined by the build process */

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
void      fate_mem_swapptr(void **a, void **b);
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



/* Video backend API : */
enum fate_vb_api {
    FATE_VB_API_DUMMY     = 0,
    FATE_VB_API_OPENGL    = 1,
    FATE_VB_API_OPENGLES  = 2,
    FATE_VB_API_VULKAN    = 3,
    FATE_VB_API_DIRECT3D  = 4,
    FATE_VB_API_MANTLE    = 5,
    FATE_VB_API_METAL     = 6,
    FATE_VB_API_GLIDE     = 7,
    FATE_VB_API_RENDERMAN = 8
};

typedef fate_gl_context HGLRC;
typedef fate_gl_context GLXContext;
typedef fate_gl_context CGLContextObj;

struct fate_vb {
    enum fate_vb_api api;
    union fate_vb_desc {
        struct fate_video_desc_gl {
            unsigned software      : 1; //Use Mesa ?
            unsigned major_version : 4;
            unsigned minor_version : 4;
            unsigned context_flags :10; //debug, forward-compatible
            unsigned profile_flags : 3; //core/compatibility;
            unsigned double_buffer : 1;
            unsigned stereo_buffer : 1;
            uint8_t  aux_buffers;
            uint8_t  red_bits;
            uint8_t  green_bits;
            uint8_t  blue_bits;
            uint8_t  alpha_bits;
            uint8_t  depth_bits;
            uint8_t  stencil_bits;
            uint8_t  accum_red_bits;
            uint8_t  accum_green_bits;
            uint8_t  accum_blue_bits;
            uint8_t  accum_alpha_bits;
        } gl;
        struct fate_video_desc_d3d {
        
        } d3d;
    } desc;
    union fate_vb_context {
        fate_gl_context gl;
    } context;
};
typedef struct fate_vb fate_vb;

void fate_vb_get_default(fate_vb *out);
/* When it fails or is NIY, the obtained backend is Dummy. */
void fate_vb_create(const fate_vb *requested, fate_vb *obtained);
void fate_vb_delete(const fate_vb *vb);
void fate_vb_use(const fate_vb *vb);
void fate_vb_copy(const fate_vb *from, fate_vb *to);


/* Virtual Filesystem API : 
 * The Node API functions require an opened shell to a VFS.
 * The rationale for shell objects :
 *     In a multithreaded execution, each thread should have
 *     its own current working node.
 *     Also, shells allow to forget about the conccurent environnment
 *     since the provided functions are atomic if needed.
 *
 * Node API functions normally alter directly the VFS.
 * However, if Engine Event is enabled, these functions send
 * messages instead.
 *
 * A shell object holds :
 * - A pointer to its VFS;
 * - A dentry stack which represents the absolute path to the current working node
 *   (similar to the concept of current working directory);
 * - An optional cache, with different locality levels, for faster path resolution;
 * - The error status of the last operation;
 *
 * A VFS holds :
 * - A memory pool for raw node data ("blocks");
 * - A memory pool for node metadata (their type, flags, and block indices);
 *
 * Features that could be provided :
 * - An inotify-like API, to react on read/writes and more 
 *   (Currently, Engine Event), enabling :
 *    - Node freezing (queuing operations until the node is unfrozen), enabling snapshots;
 *    - Deferred node-level message passing (like in Smalltalk), useful for
 *      parent-relative values.
 *      In huge hierarchies, the thread isn't forced to immediately commit.
 *      Child nodes just pull changes from the parents when needed.
 *    - Global/local journals, providing undo/redo functionality and changelogs;
 *    - A Version Control System;
 *    - Checksums/Encryption/Decryption;
 * - Users, groups and permissions ?
 * - Disk usage, Disk free;
 * - Copy-on-Write;
 * - Defragmentation (for long-term persistent worlds);
 */

void fate_vfs_init(fate_vfs *fs);
void fate_vfs_deinit(fate_vfs *fs);
void fate_sh_init(fate_sh *s, fate_vfs *fs, const char *name);
void fate_sh_deinit(fate_sh *s);

/* flags may be 0 or a bitwise-OR combination of :
 *  FATE_SH_CACHE_READ  //Read the cache while resolving paths.
 *  FATE_SH_CACHE_WRITE //Write path resolution results to the cache.
 */
void        fate_sh_flags(fate_sh *s, uint32_t flags);
uint32_t    fate_sh_get_flags(fate_sh *s);
uint32_t    fate_sh_get_last_error(fate_sh *s);
const char *fate_sh_get_error_str(uint32_t err);


/* Node API :
 *
 * To optimize caching, the caller should use sh_flags().
 * All of the functions from this API that alter the VFS require a shell object as their
 * first parameter. In cases it seems like the shell object is useless, just keep in mind
 * that at least the VFS's journal needs it.
 * When we want to prevent operations from overwriting exisiting nodes, we just check for the node's
 * existence with fate_sh_resolve() and then make decisions.
 * All operations overwrite existing nodes when they exist (that means unlink() is called if needed).
 * Please don't make assumptions based on their similarities with OS-specific file APIs/utilities.
 *
 * serialization formats for (de)serialize:
 *   FATE_SERIALIZE_JSON
 *   FATE_SERIALIZE_XML
 *   FATE_SERIALIZE_IR // "Intermediate Representation". Cross-platform binary 
 *                        representation.
 * compression formats for (de)serialize:
 *   FATE_COMPRESS_NONE = 0
 *   FATE_COMPRESS_ZIP
 *   FATE_COMPRESS_GZIP
 *   FATE_COMPRESS_BZIP2
 */

/* State execution must be atomic. 
 * sh_write_u32
 * sh_write_u64
 * ...
 * sh_read_u32
 *
 * When two fighters punch at the same time, they must both get hit.
 * There's no fighter whose punches connects first, putting the other in an
 * unexpected state before its attack resolves too.
 * 
 * So here is the "sorted transactions" state execution model :
 *  - Each state execution is wrapped in a transaction. The transaction needs
 *    to have enough info to be sorted at the end of all states executions;
 *  - Within transactions, alterations are performed on copies of 
 *    the actual nodes (Copy-On-Write);
 *  - When all entities have finished executing, each entity has its copies
 *    merged with it, in an order common to all clients.
 * Pros :
 *   - Ensures consistent states, principle of least surprise;
 *   - The threads never wait;
 *   - This model interoperates with the time travel feature.
 * Cons :
 *   - Unavoidable memory overhead;
 *   - Minor performance overhead;
 *
 * The "naive" model can be provided in case the client needs performance for
 * an offline game, which doesn't care either about attack clashes.
 *
 * There are 'hit_targets' and 'hit_sources' nodes.
 *
 * How to implement "attack clashes" ?
 * - In the attack's definition :
 *   - Set a variable like "priority" to a positive value for as long as your
 *     attack can hit. Set it to 0 as soon as it can't anymore.
 *   - When you hit another, set for them a variable like 
 *     "opponent_priority" to your "priority", then make them _execute_ your
 *     (or theirs) "pre_victim_state";
 *   - DO NOT rely on their "priority" at this time. It's outdated.
 *     In fact, in this context, you can't reliably get values, but only PUSH
 *     your own.
 * - In "pre_victim_state" :
 *   - Compare "opponent_priority" to your "priority" to decide if it's a 
 *     clash, then _change_ your state accordingly. You define your policy !
 *     If your priority is zero, then you've really taken the hit.
 *     At this point you may also want to reset "priority" 
 *     and "opponent_priority".
 * Done !
 */


/* As you would expect, it sets the current working node, returning its dentry for convenience. 
 * 'path' may be NULL. In that case, the shell's default directory is used. */
fate_dentry*   fate_sh_chdir(fate_sh *s, const uint8_t *path);
/* 
 * Returns the absolute path to the current working node, into a dynamically-allocated string.
 * Rationale for returning a dynamically-allocated string :
 *     Getting the CWN must be an atomic operation.
 *     dentries in the CWN stack might be altered by other running shells.
 *     So we can't reliably return a 'length' for the path without forcing the
 *     caller to manually lock the CWN stack.
 *     Since only the shell object is responsible for the CWN stack, we can only
 *     return atomically the path as it is at the time of the call.
 */
uint8_t*       fate_sh_getcwn(fate_sh *s);
/* pushd() is a convenience function which should not be used extensively.
 * It actually pushes the string returned by getcwn() on a stack held by the shell,
 * before returning setcwn(shell, path). The semantics are the same.
 */
fate_dentry*   fate_sh_pushd(fate_sh *s, const uint8_t *path);
/* rotatd() rotates the pushd stack. */
void           fate_sh_rotatd(fate_sh *s, int offset);
/* popd() pops the 'path' at the top of the pushd stack before returning setcwn(shell, path). */
fate_dentry*   fate_sh_popd(fate_sh *s, bool from_bottom, unsigned offset);
/* topd() returns the top of the pushd stack. */
fate_dentry*   fate_sh_topd(fate_sh *s, bool from_bottom, unsigned offset);
/* fate_sh_resolve() returns NULL if no node matches the path. */
fate_dentry*   fate_sh_resolve(fate_sh *s, const uint8_t *path);
/* This variant does not dereference the last item in the path if it is a symbolic link. */
fate_dentry*   fate_sh_resolve_nofollow(fate_sh *s, const uint8_t *path);
/* expand()'s result is a NULL-terminated array of dentry addresses.
 * The array must be fate_free()'d by the caller, not the dentries.
 * Path patterns expand to dentries, and as such, there's no guarantee that
 * the nodes are distinct; the caller is expected to handle this.
 * The 'count' out-parameter is optional. When used, it holds the returned dentries count.
 */
fate_dentry**  fate_sh_expand(fate_sh *s, const uint8_t *path_pattern, size_t *count);
/* select() is the same as expand(), except it allows more operators so the
 * pattern truly looks like an SQL 'SELECT' query.
 * Namely, it allows the presence of parentheses, '!', '|' and '&' operators.
 * Of course, it performs slower than expand() when used on normal path 
 * patterns.
 * Unlike in SQL though, 'DISTINCT' functionality is not provided, nor is
 * 'ORDER BY'.
 * As with expand(), the caller is expected to handle this.
 */
fate_dentry**  fate_sh_select(fate_sh *s, const uint8_t *path_pattern, size_t *count);
/* check_path() returns a bitfield indicating whether a path is valid and
 * conforms to certain characteristics.
 * Whenever, during parsing, the path proves to be invalid, parsing immediately
 * stops; then the range of the error is returned in 'begin_index' and 
 * 'end_index'. If the error spans to one character, 'begin_index' and 
 * 'end_index' take the same value.
 * Therefore, if the caller wishes to report all errors, they could increment 
 * 'path' by 'end_index'+1 and call check_path() again; I don't recommend it
 * though.
 * Return values :
 * - FATE_CHKPATH_ILLEGAL_CHARACTER
 * - FATE_CHKPATH_CLOSING_EXPECTED for unclosed parentheses
 * - FATE_CHKPATH_EXPR_EXPECTED when an operator expects something after it
 * - FATE_CHKPATH_ILLEGAL_NAME when a name starts by a digit, for instance
 * - FATE_CHKPATH_INVALID_PATH is a bitwise OR of all of the above.
 *                             if untrue, it is a valid path for resolve(),
 *                             expand() and select();
 * - FATE_CHKPATH_EXPAND_PATH the path is valid for expand() and select(), but
 *                            not resolve().
 * - FATE_CHKPATH_SELECT_PATH the path is valid for select(), but neither 
 *                            expand() nor resolve().
 */
uint32_t       fate_check_path(const uint8_t *path, size_t *begin_index, size_t *end_index);
/* (de)serialize's result must be fate_free()'d by the caller. 
 * The returned byte array is NULL-terminated, just like a string. 
 * It should hold magic numbers and version information. */
uint8_t*       fate_serialize(const fate_dentry *n, fate_serialization_format fmt, fate_compression_format c);
/* 'data' must point to a byte array as returned by fate_serialize(). */
fate_dentry*   fate_deserialize(const uint8_t *data);
void           fate_sh_link(fate_sh *s, fate_node *src, fate_node *dst_parent, const uint8_t *dst_name);
void           fate_sh_unlink(fate_sh *s, fate_node *parent, const uint8_t *name);
/*
 * fate_create() is a convenience function, because it is equivalent to using fate_link() 
 * with 'src' having a links count of 0.
 * It is the preferred way of adding nodes to the VFS because it is journal-friendly.
 * Do not use it to create symbolic links; use fate_symlink() instead.
 */
void           fate_sh_create(fate_sh *s, const fate_node *src, fate_node *parent, const uint8_t *name);
void           fate_sh_symlink(fate_sh *s, fate_node *parent, const uint8_t *name, const uint8_t *target);
/* Returns a dynamic copy of a Symlink's target, or NULL if the node is not a symlink. */
const uint8_t* fate_sh_readlink(fate_sh *s, const fate_node *lnk);
/* copy() is different from link() because it also _copies_ children recursively. 
 * Also, unlike with create(), the source node is assumed to already exist in the VFS. */
void           fate_sh_copy(fate_sh *s, const fate_node *src, fate_node *dst_parent, const uint8_t *dst_name);
/*
 * fate_sh_move() is a convenience function, because it is equivalent to :
 * fate_node *src = fate_sh_resolve(src_parent, src_name);
 * if(src) {
 *     fate_sh_link(src, dst_parent, dst_name);
 *     fate_sh_unlink(src_parent, src_name);
 * } else fate_logf_err("Warning : blablah\n");
 * However, it is the preferred way of moving nodes though the VFS, because it is journal-friendly.
 */
void           fate_sh_move(fate_sh *s, 
                            fate_node *src_parent, const uint8_t *src_name, 
                            fate_node *dst_parent, const uint8_t *dst_name);
/* 'new_name' must be a name, not a path. If a sibling is already named 'new_name', 
 * it is unlinked before renaming occurs. 
 * These siblings are assumed to be the 'parent' parameter's children.
 * It differs from fate_sh_move() in the sense that instead of using fate_sh_link() and fate_sh_unlink(),
 * the dentry's name is just freed and replaced by a dynamic copy of 'new_name'.
 * So it should perform faster.
 */
void           fate_sh_rename(fate_sh *s, fate_node *parent, fate_dentry *d, const uint8_t *new_name);
/* A catch-all function similar to Linux's ioctl() system call. 
 * Internal work-in-progress features or plugins could expose their own ioctls.
 */
void*          fate_sh_ioctl(fate_sh *s, fate_node *n, uint32_t op, void *arg);


/* Engine Event API : */

void                 fate_ee_mask(uint32_t mask);
uint32_t             fate_ee_get_mask(void);
void                 fate_ee_sendto(fate_ee_plugin_handle pluginh, const fate_ee_event *e);
fate_ee_watch_handle fate_ee_watch(const uint8_t *path_no_wilcards, uint32_t mask);
void                 fate_ee_unwatch(fate_ee_watch_handle wh);

