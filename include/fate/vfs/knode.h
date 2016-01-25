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

#ifndef FST_KNODE_H
#define FST_KNODE_H

#include <fst/node.h>
#include <stdint.h>

struct fst_knode_s8    { FST_NODE_ATTRIBUTES; int8_t    value; };
struct fst_knode_s16   { FST_NODE_ATTRIBUTES; int16_t   value; };
struct fst_knode_s32   { FST_NODE_ATTRIBUTES; int32_t   value; };
struct fst_knode_s64   { FST_NODE_ATTRIBUTES; int64_t   value; };
struct fst_knode_u8    { FST_NODE_ATTRIBUTES; uint8_t   value; };
struct fst_knode_u16   { FST_NODE_ATTRIBUTES; uint16_t  value; };
struct fst_knode_u32   { FST_NODE_ATTRIBUTES; uint32_t  value; };
struct fst_knode_u64   { FST_NODE_ATTRIBUTES; uint64_t  value; };
struct fst_knode_f32   { FST_NODE_ATTRIBUTES; float     value; };
struct fst_knode_f64   { FST_NODE_ATTRIBUTES; double    value; };
struct fst_knode_str8  { FST_NODE_ATTRIBUTES; uint8_t  *value; };
struct fst_knode_str32 { FST_NODE_ATTRIBUTES; uint32_t *value; };

typedef struct fst_knode_s8    fst_knode_s8 ;
typedef struct fst_knode_s16   fst_knode_s16;
typedef struct fst_knode_s32   fst_knode_s32;
typedef struct fst_knode_s64   fst_knode_s64;
typedef struct fst_knode_u8    fst_knode_u8 ;
typedef struct fst_knode_u16   fst_knode_u16;
typedef struct fst_knode_u32   fst_knode_u32;
typedef struct fst_knode_u64   fst_knode_u64;
typedef struct fst_knode_f32   fst_knode_f32;
typedef struct fst_knode_f64   fst_knode_f64;
typedef struct fst_knode_str8  fst_knode_str8;
typedef struct fst_knode_str32 fst_knode_str32;

inline fst_knode_s8    *fst_knode_s8_alloc(void);
inline fst_knode_s16   *fst_knode_s16_alloc(void);
inline fst_knode_s32   *fst_knode_s32_alloc(void);
inline fst_knode_s64   *fst_knode_s64_alloc(void);
inline fst_knode_u8    *fst_knode_u8_alloc(void);
inline fst_knode_u16   *fst_knode_u16_alloc(void);
inline fst_knode_u32   *fst_knode_u32_alloc(void);
inline fst_knode_u64   *fst_knode_u64_alloc(void);
inline fst_knode_f32   *fst_knode_f32_alloc(void);
inline fst_knode_f64   *fst_knode_f64_alloc(void);
inline fst_knode_str8  *fst_knode_str8_alloc(void);
inline fst_knode_str32 *fst_knode_str32_alloc(void);
inline void fst_knode_dealloc_extras(fst_node *n);
fst_hardlink *fst_knode_alloc_converted(fst_hardlink *n, fst_value_type t, int decimal_part_numdigits);

#endif /* FST_KNODE_H */
