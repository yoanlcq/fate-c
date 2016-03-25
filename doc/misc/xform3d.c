/* Isn't it better to have a cyberzard component ? 
 * Resolved : Basically this IS a cyberzard component.
 * They are identified by id, basic system functions are generated,
 * and the user is free to register their custom system functions too.
 *
 * Actually there is no entity.
 */

/* Automatically generated SoA. Private API. */
typedef uint16_t cj_cyberzard;
compile_time_assert(sizeof(cj_cyberzard) <= sizeof(fate_entity));
struct cj_cyberzard_s {
    cj_cyberzard count;
    fate_xform3d xform3d[CJ_MAX_CYBERZARD_COUNT];
    /* Only keep in this struct the components needed for making
     * decisions.
     * For instance, you don't have to have a fate_meshmdl array here.
     * But you can still take/give and store it into a components that needs
     * it. More in cyberzard_s_takev().*/
    fate_mesh mesh[CJ_MAX_CYBERZARD_COUNT];
};
typedef struct cj_cyberzard_s cj_cyberzard_s;
cj_cyberzard_s cj_cyberzard_s;
const fate_schema cj_cyberzard_schema;
void cj_cyberzard_s_init();
void cj_cyberzard_s_deinit();
cj_cyberzard cj_cyberzard_s_takev(cj_cyberzard *ids, cj_cyberzard count) {
    take_id();
    fate_xform3ds_take();
    fate_mesh_s_take();
}
void cj_cyberzard_s_givev(cj_cyberzard *ids, cj_cyberzard count) {
    fate_xform3ds_give(...);
    fate_meshmdl_give(fate_mesh_get_model(...));
    fate_mesh_s_give(...);
    give_id();
}


/* User's part */
void cj_cyberzard_init(cj_cyberzard id, int foo_parameter) {
    fate_meshmdl mdl = fate_meshmdl_s_take();
    fate_meshmdl_data(file("foo.data"));
    fate_mesh_model(mdl);
}
void cj_cyberzard_deinit(cj_cyberzard id) {}
void cj_cyberzard_foostate(cj_cyberzard id) {}
void cj_cyberzard_batch(fate_batchinfo *s) { /* Update all cyberzards... */ }
fate_register_systemupdate(cj_cyberzard_batch);
struct fate_batchinfo {
    size_t thread_index;
    size_t thread_count;
}

/* The root node does : */
{
    cj_cyberzard i, c[13]; //why not
    cj_cyberzard_s_takev(c, 13);
    for(i=0 ; i<13 ; ++i)
        cj_cyberzard_init(c[i], i%3);
    //Then they will manage themselves very well.
}
//And uh, also :
struct cj_weird_s {
    cj_weird count;
    cj_cyberzard cyberzard[64];
    cj_monster monster[64];
};


/* Public API */
/* fate/xform3d.h */
/*
 * TODO how to handle entities too far away to be in memory ?
 *
 * The transform3d 0 is reserved to identify the root. 
 * It is frozen by definition, and undefreezable. 
 *
 * Given a frozen transform F and a defrozen one D, 
 * if you set D as F's parent, expecting F to follow D as it moves,
 * you'll have to explicitly defreeze F. Otherwise F will just stand there.
 *
 * What happens to frozen transforms is that, while their local transform
 * can still change, the computation of their global transform is skipped,
 * so it has no effect. Thus, upon defreezing, you'll see your changes
 * take effect immediately.
 *
 * Freezing is an optimization feature, but nothing prevents the game
 * mechanics from taking advantage of it.
 * You should always freeze transforms which you know will not change.
 */
#define fate_xform3d_root() 0
void fate_xform3d_parent(fate_xform3d id, fate_xform3d p);
fate_xform3d fate_xform3d_get_parent(fate_xform3d id);
/* You are responsible for calling it. Neither freeze() nor defreeze() do
 * the check, but they assert() on it. */
bool fate_xform3d_is_frozen(fate_xform3d id);
void fate_xform3d_freeze(fate_xform3d id);
/* defreezing() is a costly operation, and there's a priori no reason to
 * use it. Don't call it just because you character will idle for some time. */
void fate_xform3d_defreeze(fate_xform3d id);
/* reset() does not call freeze() not defreeze(). It just zeroes the local
 * transform. */
void fate_xform3d_reset(fate_xform3d id);
fate_spacevec3* fate_xform3d_get_translation(fate_xform3d id);
fate_spacequat* fate_xform3d_get_rotation(fate_xform3d id);
fate_spacevec3* fate_xform3d_get_scaling(fate_xform3d id);
/* Should not be used extensively. */
fate_spacemat4* fate_xform3d_get_global_xform(fate_xform3d id);
void fate_xform3d_translate(fate_xform3d id, spacevec3 translation);
void fate_xform3d_rotate(fate_xform3d id, spacevec3 euler_angles);
void fate_xform3d_orbit(fate_xform3d id, spacevec3 center, vec3 axis, 
                        fate_space_unit angle);
void fate_xform3d_scale(fate_xform3d id, spacevec3 scale);
/* use the current translation as the 'eye' parameter. */
void fate_xform3d_lookat(fate_xform3d id, spacevec3 center, spacevec3 up);


/* fate_xform3ds.h */
#define FATE_XFORM3D_CNT 64
typedef uint16_t fate_xform3d;
struct fate_xform3dsa {
    fate_xform3d   count;
    fate_xform3d   first_defrozen;
    bool           must_sort_defrozen;
    fate_xform3d   index[FATE_XFORM3D_CNT]; /* Translate id to actual index. */
    fate_xform3d   parent[FATE_XFORM3D_CNT]; /* ids, not indices. */
    fate_spacevec3 translation[FATE_XFORM3D_CNT];
    fate_spacequat rotation[FATE_XFORM3D_CNT]; 
    fate_spacevec3 scale[FATE_XFORM3D_CNT];
    fate_spacemat4 global[FATE_XFORM3D_CNT];
};
typedef struct fate_xform3dsa fate_xform3dsa;
fate_xform3dsa fate_xform3ds = {FATE_XFORM3D_CNT, 1, 0};


/* Underlying system */
/*
 * I noticed that, very often, transforms were either set once and never updated
 * (e.g rocks) or changed every tick (e.g actors).
 * In the same way, in 99% of cases, parenting one transform to another only 
 * happens once (at child creation time), while updating the hierarchy happens
 * every tick. Therefore, we don't care if parenting is slow as long as it
 * makes update() faster thanks to better layed-out input.
 *
 * The implementation, as such, has these concepts :
 * - Frozen transforms are assumed to NEVER change.
 *   Their respective global transforms must be computed only once;
 * - Defrozen transforms are assumed to ALWAYS change.
 *   Their respective global transforms must be computed each tick;
 * - Frozen transforms that have their parent defrozen remain frozen;
 * - The root transform (which is private) is frozen by definition.
 * Considering there is no use case for an intermediate state between 
 * frozen and defrozen, we get the following benefits :
 * - For both cases, no need to check for dirty flags. Users don't need to set
 *   them either;
 * - Defrozen transforms are in their own sorted pool, which the system 
 *   directly updates without thinking (i.e no branches).
 *
 */

/* Assumes, in the range between first_defrozen and count, that no child is
 * before its parent. 
 * Additionally, it gets better as parents are closer as possible to their
 * children in memory.
 */
static void fate_xform3ds_update(void) {
    fate_xform3d i;
#define s fate_xform3ds
    if(s.must_sort_defrozen) {
        fate_xform3ds_sort_defrozen();
        s.must_sort_defrozen = false;
    }
    for(i=s.first_defrozen ; i<s.count ; ++i) {
        s.global[i] = mat4_mul(s.local[i], s.global[s.index[s.parent[i]]]);
    }
#undef s
}
static void fate_xform3ds_swap(fate_xform3d lft, fate_xform3d rgt) {
    swapmem(lft, rgt);
    fate_xform3d tmp = fate_xform3ds.index[lft];
    fate_xform3ds.index[lft] = fate_xform3ds.index[rgt];
    fate_xform3ds.index[rgt] = tmp;
}
/* Assume it was already sorted. */
static void fate_xform3ds_sort_defrozen(fate_xform3d start) {
    assert(start >= fate_xform3ds.first_defrozen);
}
bool fate_xform3d_is_frozen(fate_xform3d id) {
    fate_xform3d idx = fate_xform3ds.index[id];
    return idx && (idx < fate_xform3ds.first_defrozen);
}
void fate_xform3d_freeze(fate_xform3d id) {
    assert(!fate_xform3d_is_frozen(id));
    fate_xform3ds_swap(id, fate_xform3ds.first_defrozen);
    ++(fate_xform3ds.first_defrozen);
    /* There's no need to call fate_xform3ds_sort_defrozen(),
     * because what we literally did is removing an element from the
     * 'defrozen' array which is already sorted. */
}
/*
 * Helper schema :
 * | = first_defrozen
 * Hierarchy sorted alphabetically
 * 
 * defreeze(E):
 *
 * initial situation       : E A|B C D F G
 * --first_defrozen        : E|A B C D F G
 * swap(E, first_defrozen) : A|E B C D F G
 * memswap()               : A|B C D E F G
 */
void fate_xform3d_defreeze(fate_xform3d id) {
    assert(fate_xform3d_is_frozen(id));
}
void fate_xform3d_parent(fate_xform3d id, fate_xform3d p) {
    fate_xform3d idx = fate_xform3ds.index[id];
    fate_xform3d pidx = fate_xform3ds.index[p];
    fate_xform3ds.parent[idx] = p;
    if( idx < fate_xform3ds.first_defrozen 
    || pidx < fate_xform3ds.first_defrozen)
        return;
    fate_xform3ds.must_sort_defrozen = true;
}

