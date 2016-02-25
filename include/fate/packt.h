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

/*! \file fate/packt.h
 *  \brief Packets and serialization.
 *  \defgroup packt Packt : DRY packets and serialization.
 *
 * This module is yet to be implemented.
 *
 * \c example.h
 * \code{.c}
 * #ifndef EXAMPLE_H
 * #define EXAMPLE_H
 *
 * #include <fate/packt.h>
 *
 * FATE_PACKTDEF_PROTO(example);
 *
 * #endif //EXAMPLE_H
 * \endcode
 *
 * \c example.c
 * \code{.c}
 * #include <fate/packt.h>
 * #include "example.h"
 *
 * FATE_PACKTDEF(
 *     example
 * ,
 *     const char *name;
 *     fate_allocator_id name_allocator;
 *     int vec4[4];
 *     example *children;
 *     size_t num_children;
 *     example *list;
 *     unsigned bit0 : 1;
 *     unsigned bit1 : 1;
 *     const char *optional_hash;
 *     int reserved;
 *     example **plane;
 *     size_t plane_w, plane_h;
 *     example **array_of_null_terminated;
 *     size_t arrnt_cnt;
 *     example **list_of_arrays;
 *     size_t loa_arraysize;
 * ,
 *     {
 *         sizeof(example),
 *         offsetof(example, name),
 *         offsetof(example, name_allocator),
 *         offsetof(example, vec4),
 *         offsetof(example, children),
 *         offsetof(example, num_children),
 *         offsetof(example, list),
 *         offsetof(example, bit0),
 *         offsetof(example, bit1),
 *         offsetof(example, optional_hash),
 *         offsetof(example, reserved),
 *         offsetof(example, plane),
 *         offsetof(example, plane_w),
 *         offsetof(example, plane_h),
 *         offsetof(example, array_of_null_terminated),
 *         offsetof(example, arrnt_cnt),
 *         offsetof(example, list_of_arrays),
 *         offsetof(example, loa_arraysize)
 *     }
 * ,
 *     // Within an 'array' attribute, a dimension of size 0 means
 *     // "This dimension is NULL-terminated".
 *
 *     // The following line has no identifier before the ':', which means
 *     // it sets the default behaviour for all fields.
 *     " : array(0) allocated(fate_mem)\n"
 *
 *     "name : array(0) allocated(:name_allocator)\n"
 *
 *     // The following line is useless in the present case, but if 'vec4'
 *     // was declared as a pointer, pointing to static memory, then it
 *     // would be needed.
 *     "vec4 : array(4) allocated(static)\n"
 *
 *     "children : array(num_children)\n"
 *     "list : array(0)\n"
 *     "optional_hash : array(0) ignored_if_equals(0)\n"
 *     "reserved : ignored\n"
 *     "plane : array(plane_h, plane_w) allocated(std, fate_mem)\n"
 *     "array_of_null_terminated : array(arrnt_cnt, 0) allocated(fate_mem)\n"
 *     "list_of_arrays : array(0, loa_arraysize) allocated(static, fate_mem)\n"
 * );
 * \endcode
 *
 * Then, use it like so :
 * \code{.c}
 * #include <stdbool.h>
 * #include <fate/packt.h>
 * #include "example.h"
 *
 * void test(bool sending) {
 *     example foo;
 *     void *data;
 *     size_t data_len;
 *     unsigned long flags = FATE_PACKT_JSON | FATE_PACKT_MAGIC;
 *     
 *     if(sending) {
 *         // Initialise 'foo'. Then :
 *         fate_pack(example, &data, &data_len, &foo, flags);
 *         send_to_friends(data, data_len);
 *         fate_mem_free(data);
 *     } else {
 *         // We should have received 'data' and 'data_len'.
 *         fate_unpack(example, &foo, data, data_len, flags);
 *         // We have now received 'foo' !
 *     }
 * }
 * \endcode
 *
 * @{
 */



#ifndef FATE_PACKT_H
#define FATE_PACKT_H


#include <fate/defs.h>

struct fate_packtdef {
    const char *name;
    const char *fields;
    const char *attributes;
    unsigned id; /*!< For faster lookups */
};
typedef fate_packtdef fate_packtdef;

#define FATE_PACKT_GETDEF(type) type##_pktdef

#define FATE_PACKTDEF_PROTO(type) \
    struct type; typedef struct type type; \
    extern fate_packtdef FATE_PACKT_GETDEF(type)

#define FATE_PACKTDEF(type, fields, attributes) \
    struct type { fields }; \
    fate_packt_def FATE_PACKT_GETDEF(type) = \
        { XSTRINGIFY(type), XSTRINGIFY(fields), attributes, 0}

bool  fate_pack_real(fate_packt_def *d, void **out_mem_malloced_data,
                     size_t *out_data_len, const void *obj, long flags);
#define fate_pack(type, ...) \
        fate_pack_real(FATE_PACKT_GETDEF(type), __VA_ARGS__)
bool  fate_unpack_real(fate_packt_def *d, void *out_obj, 
                      const void *data, size_t data_len, long flags);
#define fate_unpack(type, ...) \
        fate_unpack_real(FATE_PACKT_GETDEF(type), __VA_ARGS__)

/* @} */

#endif /* FATE_PACKT_H */
