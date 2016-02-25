typedef uint64_t fate_time_unit;
typedef fate_time_unit fate_tick;
typedef int64_t fate_space_unit;

struct fate_transform4d {
    fate_space_vec3 pos, vel, accel;
    quat rot, rot_vel, rot_accel;
    vec3 scale, scale_vel, scale_accel;
    fate_time_unit time, time_vel, time_accel;
};

struct fate_world {
    fate_time_unit time, time_vel, time_accel;
    fate_region **regions;
    fate_portal **portals;
    fate_entity **entities;
    fate_view   **views;
};
struct fate_rootworld {
    fate_time_unit time, time_vel, time_accel;
    fate_region **regions; /* There can be only regions in the root world. */
};

enum fate_face {
    FATE_FACE_FRONT = 0,
    FATE_FACE_BACK  = 1
};
typedef enum fate_face fate_face;
struct fate_portal_dest {
    fate_portal *portal;
    fate_face face;
};
struct fate_portal {
    float opacity;
    /* It might not be just a regular plane. 
     * Shapes need not match between portals. Passing into the
     * other side is just defined as a position relative to centers. */
    fate_shape2d shape;
    fate_transform4d transform;
    /* Access with a fate_face as the array index. */
    fate_portal_dest destinations[2];
};

/* TODO be able to render to textures, using renderspecs.
 * This would give us the ability to simulate screens. */

struct fate_region_boundaries {
    fate_space_unit left, right, bottom, top, near, far;
    /* Say whether we allow wrapping around or not.
     * Thus, we can choose to prevent objects from going outside
     * of the region. */
    unsigned wrap_left :1;
    unsigned wrap_right :1;
    unsigned wrap_bottom :1;
    unsigned wrap_top :1;
    unsigned wrap_near :1;
    unsigned wrap_far :1;
    unsigned reserved :2;
};
struct fate_region {
    /* Relative to the parent coords. */
    fate_space_unit half_edge_size;
    /* Same here. */
    fate_space_vec3 center;
    /* Boundaries, upon which coordinates should wrap around. 
     * The region's resolution is still fate_space_unit as always. */
    fate_region_boundaries boundaries;
    /* Two regions can point to the same world.
     * Put another way, a same world can be accessed from more than one
     * region.
     * That's very interesting. It may allow for fractal worlds. */
    fate_world *world;
    /* Another cool thing to note is that coordinates should never be
     * checked against limits, and just wrap around. Therefore, when
     * approaching a region's limit, you see the other side. And if
     * you go further, you'll just be positioned at the other side. */
};

struct fate_view_perspective_params {
    float fov_y, ratio, near, far;
};
struct fate_view_ortho_params {
    float left, right, bottom, top, near, far;
};
struct fate_view_frustum_params {
    float left, right, bottom, top, near, far;
};
enum fate_view_mode {
    FATE_VIEW_MODE_PERSPECTIVE = 1,
    FATE_VIEW_MODE_ORTHO = 2,
    FATE_VIEW_MODE_FRUSTUM = 3
};
union fate_view_mode_params {
    struct fate_view_perspective_params perspective;
    struct fate_view_ortho_params ortho;
    struct fate_view_frustum_params frustum;
};
enum fate_lodset_testfunc {
     FATE_LODSET_LESS,     /* Keep only the lower lods. This is the default. */
     FATE_LODSET_GREATER /* Keep only the greater lods. */
};
struct fate_view {
    /* FEATURE : ability to focus on a very far location 
     * (both in visibility and audibility). */
    fate_transform4d transform;
    enum  fate_view_mode mode;
    union fate_view_mode_params mode_params;
    /* The two following lodsets are computed per the view. 
     * lodset = a set of Level-Of-Details. 
     * A zero LOD means the object is culled. */
    fate_visibility_lodset visibility_lodset;
    fate_audibility_lodset audibility_lodset;
    /* There's the possibility to add extra lodsets, whether they are from 
     * other views or just added arbitrarily
     * (ex. A creepy ghost seen in one view, but not the other).
     * The extra lodsets are merged with the our lodsets, using a test function,
     * forming the final lodsets, which are not supposed to be written to
     * directly, and are only computed when dirty (think of them as caches). */
    fate_visibility_lodset *extra_visibility_lodsets;
    fate_audibility_lodset *extra_audibility_lodsets;
    /* Tell how the lods contribute to the final lodsets. */
    fate_lodset_testfunc   visibility_lodset_testfunc;
    fate_lodset_testfunc   audibility_lodset_testfunc;
    fate_visibility_lodset final_visibility_lodset;
    fate_audibility_lodset final_audibility_lodset;
};

/* Works on the 3D view of the world. */
struct fate_fx3d {
    float fanciness_hint; /* Mind-blowing graphics or not ? */
    fate_fx3d_fog fog;
    fate_fx3d_dof depth_of_field;
    fate_fx3d_3dvision stereoscopy;
    fate_fx3d_bokeh bokeh;
    fate_fx3d_lensflare lensflare;
    fate_fx3d_taa temporal_anti_aliasing;
    /* FEATURE : support entity-level FX ?
     * Clipping mask support would be cool. */
};
/* Inspired by Krita's Filter Layers. */
enum fate_fx2d_filter_type {
    FATE_FILTER_LEVELS,
    FATE_FILTER_BRIGHTNESS_CONTRAST,
    FATE_FILTER_HSL_ADJUST,
    FATE_FILTER_INVERT,
    FATE_FILTER_POSTERIZE,
    FATE_FILTER_GRADIENT_MAP,
    FATE_FILTER_COLOR_REMAP,
    FATE_FILTER_GAUSSIAN_BLUR,
    FATE_FILTER_MOTION_BLUR,
    FATE_FILTER_COLOR_TO_ALPHA,
    FATE_FILTER_NORMALIZE
};
struct fate_fx2d_filter_layer {
    float alpha;
    enum fate_fx2d_filter_type type;
    union fate_fx2d_filter_union un;
};
/* Works on the 2d rendered image. */
struct fate_fx2d {
    fate_filter_layer *filter_layers;
    fate_fx2d_crop crop;
};
struct fate_renderspec {
    fate_fx3d *fx3d;
    fate_fx2d *fx2d;
    fate_view *view;
};

struct fate_window {
    SDL_window *win;
    /* Other cached data, maybe even supported video backends... */
};

struct fate_viewport {
    /* Decoupled since it may be used to render to textures. */
    fate_renderspec renderspec;
    fate_videobackend *vb;
    /* Viewport rect, in window coordinates. */
    uvec2 pos, size;
    float alpha;
    /* The Global State has a fate_userdevice_feedback object which holds
     * the viewports, sorted by z-index, into a list. 
     * The engine iterates over them.
     * The z-index setter will just change
     * the viewport's position within the list, and set the z_index field 
     * so it can be remembered for future comparisons. */
    uint32_t z_index;
    fate_window *windows;
    size_t num_windows;
};

struct fate_viewport_linkedlist {
    fate_viewport *vp, *next;
};

/* Everything needed to give a result to the user. */
struct fate_userdevice_feedback {
    fate_viewport_linkedlist *viewport_list; /* sorted by z-index. */
    fate_hapticbackend *haptic;
    fate_audiobackend *audio;
};
struct fate_videobackend_hub {};
