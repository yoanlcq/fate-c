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

#ifndef FST_GLOBALTREE_H
#define FST_GLOBALTREE_H

#include <fst/node.h>
#include <fst/nodes.h>
#include <fst/lnode.h>
#include <fst/knode.h>
#include <fst/dnode.h>
#include <fst/enode.h>
#include <fst/vnode.h>

struct fst_globaltree {
    fst_lnode *root;
};
typedef struct fst_globaltree fst_globaltree;

extern fst_globaltree *fst_used_globaltree;

inline void fst_globaltree_init  (fst_globaltree *g);
inline void fst_globaltree_deinit(fst_globaltree *g);
inline void fst_globaltree_use   (fst_globaltree *g);

bool             fst_checkpath          (const uint8_t *path);
fst_nodequeue    fst_res                (const uint8_t *path);
fst_nodequeue    fst_res_nofollow       (const uint8_t *path);
fst_symlink*     fst_res_single_nofollow(const uint8_t *path);
void*            fst_res_single         (const uint8_t *path);

#define fst_resn(path)       ((fst_node        *) fst_res_single(path))
#define fst_resh(path)       ((fst_hardlink    *) fst_res_single(path))
#define fst_resk_s8(path)    ((fst_knode_s8    *) fst_res_single(path))
#define fst_resk_s16(path)   ((fst_knode_s16   *) fst_res_single(path))
#define fst_resk_s32(path)   ((fst_knode_s32   *) fst_res_single(path))
#define fst_resk_s64(path)   ((fst_knode_s64   *) fst_res_single(path))
#define fst_resk_u8(path)    ((fst_knode_u8    *) fst_res_single(path))
#define fst_resk_u16(path)   ((fst_knode_u16   *) fst_res_single(path))
#define fst_resk_u32(path)   ((fst_knode_u32   *) fst_res_single(path))
#define fst_resk_u64(path)   ((fst_knode_u64   *) fst_res_single(path))
#define fst_resk_f32(path)   ((fst_knode_f32   *) fst_res_single(path))
#define fst_resk_f64(path)   ((fst_knode_f64   *) fst_res_single(path))
#define fst_resk_str8(path)  ((fst_knode_str8  *) fst_res_single(path))
#define fst_resk_str32(path) ((fst_knode_str32 *) fst_res_single(path))
#define fst_resd(path)       ((fst_dnode       *) fst_res_single(path))
#define fst_rese(path)       ((fst_enode       *) fst_res_single(path))
#define fst_resv(path)       ((fst_vnode       *) fst_res_single(path))


#endif /* FST_GLOBALTREE_H */
