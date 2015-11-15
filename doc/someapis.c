/* Utils API : */

inline void F8_memswap1(uint8_t  *a, uint8_t  *b);
inline void F8_memswap2(uint16_t *a, uint16_t *b);
inline void F8_memswap4(uint32_t *a, uint32_t *b);
inline void F8_memswap8(uint64_t *a, uint64_t *b);
inline void F8_memswapN(void *a, void *b, size_t s);

void F8_memreverse(void *p, uint64_t nbytes);


/* Filesystem API : */

void F8_vfs_init(F8_vfs *fs);
void F8_vfs_make_current(F8_vfs *fs);
void F8_vfs_deinit(F8_vfs *fs);


/* Node API (requires a current vfs) :
 * flags for resolve*():
 *   F8_RESOLVE_NO_FOLLOW
 *   F8_RESOLVE_NO_CACHE_READ
 *   F8_RESOLVE_NO_CACHE_WRITE
 *   F8_RESOLVE_NO_CACHE //Combination of both above.
 */

F8_node*        F8_resolve(const uint8_t *path, uint32_t flags);
F8_nodelist*    F8_resolve_multiple(const uint8_t *path, uint32_t flags);
void            F8_symlink(const uint8_t *oldpath, const uint8_t *newpath);
const uint8_t*  F8_readlink(const uint8_t *path);
void            F8_create(const uint8_t *path, const F8_node *initializer);
void            F8_link(const uint8_t *oldpath, const uint8_t *newpath);
void            F8_unlink(const uint8_t *path);
void            F8_copy(const uint8_t *srcpath, const uint8_t *dstpath);
void            F8_move(const uint8_t *srcpath, const uint8_t *dstpath);
void            F8_rename(const uint8_t *srcpath, const uint8_t *newname);
F8_nodestat     F8_stat(const uint8_t *path);
void            F8_ioctl(const uint8_t *path, uint32_t op, ...);
uint8_t*        F8_serialize(const uint8_t *path, uint32_t flags);
F8_node*        F8_deserialize(const uint8_t *stream, uint32_t flags);

F8_node*        F8_resolveat(F8_node *parent, const uint8_t *path, uint32_t flags);
F8_nodelist*    F8_resolveat_multiple(F8_node *parent, const uint8_t *path, uint32_t flags);
void            F8_symlinkat(const uint8_t *oldpath, F8_node *dst_parent, const uint8_t *newpath);
const uint8_t*  F8_readlinkat(const F8_node *parent, const uint8_t *path);
void            F8_createat(F8_node *parent, const uint8_t *path, const F8_node *initializer);
void            F8_linkat(F8_node *oldparent, const uint8_t *oldpath, F8_node *newparent, const uint8_t *newpath);
void            F8_unlinkat(F8_node *parent, const uint8_t *path);
void            F8_copyat(F8_node *oldparent, const uint8_t *oldpath, F8_node *newparent, const uint8_t *newpath);
void            F8_moveat(F8_node *oldparent, const uint8_t *oldpath, F8_node *newparent, const uint8_t *newpath);
void            F8_renameat(F8_node *oldparent, const uint8_t *oldpath, const uint8_t *newname);
F8_statdata     F8_statat(F8_node *parent, const uint8_t *path);
void            F8_ioctlat(F8_node *parent, const uint8_t *path, uint32_t op, ...);

void            F8_symlinkfromat(F8_node *src, F8_node *dst_parent, const uint8_t *newpath);
const uint8_t*  F8_readlinkfrom(const F8_symlink *lnk);
void            F8_linkfrom(F8_node *src, const uint8_t *newpath);
void            F8_linkfromat(F8_node *src, F8_node *newparent, const uint8_t *newpath);
void            F8_unlinkfrom(F8_node *src);
void            F8_copyfrom(F8_node *src, const uint8_t *newpath);
void            F8_copyfromat(F8_node *src, F8_node *newparent, const uint8_t *newpath);
void            F8_movefrom(F8_node *src, const uint8_t *dstpath);
void            F8_movefromat(F8_node *src, F8_node *newparent, const uint8_t *newpath);
void            F8_renamefrom(F8_node *src, const uint8_t *newname);
F8_statdata     F8_statfrom(F8_node *src);
void            F8_ioctlfrom(F8_node *src, uint32_t op, ...);


/* Engine Event API : */

void                F8_ee_mask(uint32_t mask);
uint32_t            F8_ee_get_mask(void);
void                F8_ee_sendto(F8_ee_mod_handle modh, const F8_ee_event *e);
F8_ee_watch_handle  F8_ee_watch(const uint8_t *path_no_wilcards, uint32_t mask);
void                F8_ee_unwatch(F8_ee_watch_handle wh);

