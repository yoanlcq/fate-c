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

#include <fst/config.h>
#include <fst/lnode.h>

#include <stdio.h>
#include <stdint.h>

#define FST_LNODE_INIT_ADD_NODE(__nodestack__,__var__,__type__,__name__) \
    __type__ *__var__ = malloc(sizeof(__type__)); \
    __var__->base.entry_type  = FST_HARDLINK; \
    __var__->base.type        = FST_NODE; \
    __var__->base.value_type  = FST_KNODE_NONE; \
    __var__->base.role        = FST_ROLE_SCHEDULER; \
    __var__->base.access      = FST_READ; \
    __var__->base.links_count = 1; \
    __var__->base.name = malloc(strlen(__name__)+1); \
    strcpy(__var__->base.name, __name__); \
    __var__->value = __val__; \
    fst_nodestack_push(__nodestack__, ln_##__type__);


#define FST_LNODE_INIT_ADD_KNODE(__nodestack__,__var__,__type__,__name__,__VALUE_TYPE__,__val__) \
    FST_LNODE_INIT_ADD_NODE(__nodestack__,__var__,__type__,__VALUE_TYPE__,__name__); \
    __var__->base.type  = FST_KNODE; \
    __var__->base.value_type = __VALUE_TYPE__; \
    __var__->base.value = __val__;


inline fst_lnode *fst_lnode_alloc (void) {
    fst_lnode *ln = malloc(sizeof(fst_lnode));
    ln->base.entry_type  = FST_HARDLINK;
    ln->base.type        = FST_LNODE;
    ln->base.value_type  = FST_KNODE_NONE;
    ln->base.role        = FST_ROLE_SCHEDULER;
    ln->base.access      = FST_READWRITE;
    ln->base.links_count = 1;
    ln->base.name = malloc(2);
    strcpy(ln->base.name, "%");
    ln->base.children.nodes = NULL;
    ln->base.children.count = 0;

    FST_LNODE_INIT_ADD_KNODE(ln->base.children, ln_s8,    fst_knode_s8,    "s8" ,  FST_KNODE_S8,  0);
    FST_LNODE_INIT_ADD_KNODE(ln->base.children, ln_s16,   fst_knode_s16,   "s16",  FST_KNODE_S16, 0);
    FST_LNODE_INIT_ADD_KNODE(ln->base.children, ln_s32,   fst_knode_s32,   "s32",  FST_KNODE_S32, 0);
    FST_LNODE_INIT_ADD_KNODE(ln->base.children, ln_s64,   fst_knode_s64,   "s64",  FST_KNODE_S64, 0LL);
    FST_LNODE_INIT_ADD_KNODE(ln->base.children, ln_u8,    fst_knode_u8,    "u8" ,  FST_KNODE_U8,  0U);
    FST_LNODE_INIT_ADD_KNODE(ln->base.children, ln_u16,   fst_knode_u16,   "u16",  FST_KNODE_U16, 0U);
    FST_LNODE_INIT_ADD_KNODE(ln->base.children, ln_u32,   fst_knode_u32,   "u32",  FST_KNODE_U32, 0U);
    FST_LNODE_INIT_ADD_KNODE(ln->base.children, ln_u64,   fst_knode_u64,   "u64",  FST_KNODE_U64, 0ULL);
    FST_LNODE_INIT_ADD_KNODE(ln->base.children, ln_f32,   fst_knode_f32,   "f32",  FST_KNODE_F32, 0.0f);
    FST_LNODE_INIT_ADD_KNODE(ln->base.children, ln_f64,   fst_knode_f64,   "f64",  FST_KNODE_F64, 0.0);
    FST_LNODE_INIT_ADD_KNODE(ln->base.children, ln_str8,  fst_knode_str8,  "str8", FST_KNODE_STR8,  NULL);
    FST_LNODE_INIT_ADD_KNODE(ln->base.children, ln_str32, fst_knode_str32, "str32",FST_KNODE_STR32, NULL);
    FST_LNODE_INIT_ADD_NODE (ln->base.children, wn, fst_hardlink, "window");
    FST_LNODE_INIT_ADD_KNODE(wn->base.children, wn_x, fst_knode_u16, FST_KNODE_U16, "x", 0);
    FST_LNODE_INIT_ADD_KNODE(wn->base.children, wn_y, fst_knode_u16, FST_KNODE_U16, "y", 0);
    FST_LNODE_INIT_ADD_KNODE(wn->base.children, wn_w, fst_knode_u16, FST_KNODE_U16, "width", 640);
    FST_LNODE_INIT_ADD_KNODE(wn->base.children, wn_h, fst_knode_u16, FST_KNODE_U16, "height", 480);
    FST_LNODE_INIT_ADD_KNODE(wn->base.children, wn_b, fst_knode_u8, FST_KNODE_U8, "bits_per_pixel", 32);
    FST_LNODE_INIT_ADD_KNODE(wn->base.children, wn_t, fst_knode_str32, FST_KNODE_STR32, "title", NULL);
    wn_t.value = malloc(4*9);
    const uint32_t default_name = {'F','S','T',' ','G','a','m','e','\0'};
    memcpy(wn_t.value, default_name, 4*9);
    FST_LNODE_INIT_ADD_KNODE(wn->base.children, wn_v, fst_knode_u8, FST_KNODE_U8, "vsync_is_enabled", 0);
    FST_LNODE_INIT_ADD_KNODE(wn->base.children, wn_r, fst_knode_u16, FST_KNODE_U16, "refresh_rate", 60);
    "window.video_backend.current"
    "window.video_backend.no_video"
    "window.video_backend.OpenGL"
    "window.video_backend.Vulkan"
    "window.video_backend.is_accelerated"
    "%.audio_backend"
    "%.audio_backend.current"
    "%.audio_backend.no_audio"
    "%.audio_backend.OpenAL"
    FST_LNODE_INIT_ADD_NODE (wn->base.children, cc, fst_hardlink, "clear_color");
    FST_LNODE_INIT_ADD_KNODE(cc->base.children, fst_knode_f32, "r", FST_KNODE_F32, 1.0f);
    FST_LNODE_INIT_ADD_KNODE(cc->base.children, fst_knode_f32, "g", FST_KNODE_F32, 0.0f);
    FST_LNODE_INIT_ADD_KNODE(cc->base.children, fst_knode_f32, "b", FST_KNODE_F32, 1.0f);
    FST_LNODE_INIT_ADD_KNODE(cc->base.children, fst_knode_f32, "a", FST_KNODE_F32, 1.0f);
}

inline void fst_lnode_dealloc_extras(fst_lnode *ln) {}
