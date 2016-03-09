/* 
 * This file is part of F.A.T.E - the Fight And Travel game Engine.
 * Copyright (c) 2015-2016 Yoan Lecoq (yoanlecoq.io at gmail.com)
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented;
 *    you must not claim that you wrote the original software. 
 *    If you use this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, 
 *    and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * Contributors :
 * - Please make sure your indentation is set to 4 spaces;
 * - Please avoid exceeding 80 character columns.
 *
 */

/*! \file fate/videostuff.h
 *  \brief TODO
 *  \defgroup videostuff Videostuff : TODO
 *
 * TODO
 * Possible FATE 2 extensions :
 * Well, that's equivalent to shipping Blender and GCC with the game.
 * procedural_terrain_extension
 * procedural_entities_extension
 *
 * @{
 */



#ifndef FATE_VIDEOSTUFF_H
#define FATE_VIDEOSTUFF_H

/*!\brief TODO  */
struct fate_transform5d {
    spacevec3 pos, vel, accel;
    spacequat rot, rot_vel, rot_accel;
    spacevec3 scale, scale_vel, scale_accel;
    fate_time_unit time, time_vel, time_accel;
    float alpha, alpha_vel, alpha_accel;
};
struct fate_transform4d {
    spacevec3 pos, vel, accel;
    spacequat rot, rot_vel, rot_accel;
    spacevec3 scale, scale_vel, scale_accel;
    fate_time_unit time, time_vel, time_accel;
};


/*!\brief TODO  */
struct fate_world {
    fate_time_unit time, time_vel, time_accel;
    fate_region **regions;
    fate_portal **portals;
    fate_entity **entities;
    fate_view   **views;
};
/*!\brief TODO  */
struct fate_rootworld {
    fate_time_unit time, time_vel, time_accel;
    fate_region **regions; /* There can be only regions in the root world. */
};

/*!\brief TODO  */
enum fate_face {
    FATE_FACE_FRONT = 0,
    FATE_FACE_BACK  = 1
};
typedef enum fate_face fate_face;
/*!\brief TODO  */
enum fate_face_bitfield {
    FATE_FACE_FRONT_BIT = 1,
    FATE_FACE_BACK_BIT  = 2
};
typedef enum fate_face_bitfield fate_face_bitfield;

/*!\brief TODO  */
struct fate_portal_dest {
    fate_portal *portal;
    fate_face face;
};
/*!\brief TODO  */
struct fate_portal {
    float opacity;
    /* It might not be just a regular plane. 
     * Shapes need not match between portals. Passing into the
     * other side is just defined as a position relative to centers. 
     * The dev may want to provide collision detection volumes. */
    fate_shape2d shape;
    fate_transform5d transform;
    /* Access with a fate_face as the array index. */
    fate_portal_dest destinations[2];
};

struct fate_entity_data {
    fate_collision_data collisions;
    fate_texture texture; /* fate_texture must support video. */
};

struct fate_entity_instance {
    fate_entity_data data;
    fate_transform5d transform;
    fate_face_bitfield visible_sides : 2;
    fate_entity_fx fx;
    /* TODO view-specific fx */
};

/* TODO be able to render to textures, using renderspecs.
 * This would give us the ability to simulate screens. */

/*!\brief TODO  */
struct fate_region_boundaries {
    fate_space_unit left, right, bottom, top, near, far;
    /* FIXME Isn't it the job of portals ? 
     * RESOLVED : It's definitely the job of portals ! Think about how
     * regions behave inside another. You can actually go out of a region
     * to end up inside its parent.
     * For the regions of the root world, just let coordinates wrap around.
     * The dev can simply place textured walls/portals depending on the desired
     * behaviour.
     *
     * Old Discussion : Yes, but for most use cases, this is easier to manage 
     * than several portals, which need to carefully match the region's faces.
     * Also, nothing prevents the recursive rendering implementation to
     * treat them as portals. */
    /* Wrapping is easy to understand : Unwrapped means that objects
     * may freely go out of a region (unless it is a region of the 
     * root world, in which case the objects are denied from going further).
     * Wrapped means that objects attempting to go out of the region will
     * reappear at the other side (integer coordinates just wrap around).
     * As a desired consequence, the other side of a region is seen/heard when
     * approaching the limits. Typical use case : Simulate a universe so huge
     * that the player doesn't notice it wraps around.
     */
    /*
    unsigned wrap_left   : 1;
    unsigned wrap_right  : 1;
    unsigned wrap_bottom : 1;
    unsigned wrap_top    : 1;
    unsigned wrap_near   : 1;
    unsigned wrap_far    : 1;
    unsigned reserved    : 2;
    */
};
/*!\brief TODO  */
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
     * you go further, you'll just be positioned at the other side. 
     * If you don't wish this behaviour, just put a texture plane with
     * a collision to prevent entities from moving through it. */
    /* The three flags below indicate whether 
     * the world should be interpreted as a symmetry. 
     * Useful for mirror modes. */
    unsigned flip_x : 1;
    unsigned flip_y : 1;
    unsigned flip_z : 1;
    unsigned reserved : 5;
};

/*!\brief TODO  */
struct fate_view_perspective_params {
    fate_space_unit fov_y, ratio, near, far;
};
/*!\brief TODO  */
struct fate_view_ortho_params {
    fate_space_unit left, right, bottom, top, near, far;
};
/*!\brief TODO  */
struct fate_view_frustum_params {
    fate_space_unit left, right, bottom, top, near, far;
};
/*!\brief TODO  */
enum fate_view_mode {
    FATE_VIEW_MODE_PERSPECTIVE = 1,
    FATE_VIEW_MODE_ORTHO = 2,
    FATE_VIEW_MODE_FRUSTUM = 3
};
/*!\brief TODO  */
union fate_view_mode_params {
    struct fate_view_perspective_params perspective;
    struct fate_view_ortho_params ortho;
    struct fate_view_frustum_params frustum;
};
/*!\brief TODO  */
enum fate_lodset_testfunc {
     FATE_LODSET_LESS,   /* Keep only the lower lods. This is the default. */
     FATE_LODSET_GREATER /* Keep only the greater lods. */
};
/*!\brief TODO  */
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
/*!\brief TODO  */
struct fate_view_3dfx {
    float fanciness_hint; /* Mind-blowing graphics or not ? */
    fate_view_3dfx_fog fog;
    fate_view_3dfx_dof depth_of_field;
    fate_view_3dfx_3dvision stereoscopy;
    fate_view_3dfx_bokeh bokeh;
    fate_view_3dfx_lensflare lensflare;
    fate_view_3dfx_taa temporal_anti_aliasing;
    /* FEATURE : support entity-level FX ?
     * Clipping mask support would be cool. */
};
/* Inspired by Krita's Filter Layers. */
/*!\brief TODO  */
enum fate_view_2dfx_filter_type {
    FATE_FILTER_FILL, /* Trivial to implement and crazy fast. 
                       * Just clear the screen. */
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
/*!\brief TODO  */
struct fate_view_2dfx_filter_layer {
    float alpha;
    enum fate_view_2dfx_filter_type type;
    union fate_view_2dfx_filter_union un;
};
/* Works on the 2d rendered image. */
/*!\brief TODO  */
struct fate_view_2dfx {
    fate_filter_layer *filter_layers;
    fate_view_2dfx_crop crop;
    fate_view_2dfx_symmetry symmetry;
    /* keeps old frames in a ring buffer, and draws them with reduced 
     * opacity. */
    fate_view_2dfx_onionskin onionskin;
    fate_view_2dfx_blendmode blendmode;
    /* TODO allow clipping masks */
};
/*!\brief TODO  */
struct fate_renderspec {
    /* TODO allow game logic to specify a timeout for effects
     * (so they don't depend on the tickrate) */
    fate_view_3dfx *fx3d;
    fate_view_2dfx *fx2d;
    fate_view *view;
};

/*!\brief TODO  */
struct fate_window {
    SDL_window *win;
    /* Other cached data, maybe even supported video backends... */
};

/*!\brief TODO  */
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

/* Everything needed to give a result to the user. */
/*!\brief TODO  */
struct fate_userdevice_feedback {
    fate_viewport *viewport_list; /* sorted by z-index. */
    fate_hapticbackend *haptic;
    fate_audiobackend *audio;
};
/*!\brief TODO  */
struct fate_videobackend_hub {};

#endif /* FATE_VIDEOSTUFF_H */

/* @} */
