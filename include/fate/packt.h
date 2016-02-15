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
 * TODO How is the behaviour defined with pointers, and arrays ?
 *
 * \c example.h
 * \code{.c}
 * #ifndef EXAMPLE_H
 * #define EXAMPLE_H
 *
 * #include <fate/packt.h>
 *
 * FATE_PACKT_ABLE_PROTO(example);
 *
 * #endif //EXAMPLE_H
 * \endcode
 *
 * \c example.c
 * \code{.c}
 * #include <fate/packt.h>
 * #include "example.h"
 *
 * FATE_PACKT_ABLE(example,
 *     const char *name;
 *     int id;
 *     example *next;
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
 *     int val;
 *     example foo, bar;
 *     char *data;
 *     char txt[32];
 *     char *unknown;
 *     size_t unknown_len;
 *     size_t data_len;
 *     
 *     if(sending) {
 *         // Initialize 'val', 'txt', 'foo' and 'bar'.
 *         // Also initialise 'unknown' and 'unkown_len'.
 *         // Like unknown_len = strlen(unknown);
 *     } else {
 *         // We should have received 'data' and 'data_len'.
 *     }
 *     
 *     fate_packt p;
 *     fate_packt_init(&p, 0); //The size hint is optional.
 *     fate_packt_addobj(&p, int, &val);
 *     fate_packt_addobjv(&p, char, txt, 32);
 *     fate_packt_addobjvn(&p, char, &unknown, &unknown_len);
 *     fate_packt_addobj(&p, example, &foo);
 *     fate_packt_addobj(&p, example, &bar);
 *     fate_packt_databuf(&p, &data, &data_len);
 *     fate_packt_flags(&p, FATE_PACKT_JSON | FATE_PACKT_GZIP2
 *          // | FATE_PACKT_MAGIC if we want a magic number.
 *     );
 *     (sending ? fate_packt_pack : fate_packt_unpack)(&p);
 *     fate_packt_deinit(&p);
 *      
 *     if(sending) {
 *         send_to_friends(data, data_len);
 *         fate_mem_free(data);
 *     } else {
 *         // You can now use 'val', 'txt', 'foo' and 'bar'.
 *         // Their lifetime is yours to manage.
 *         // You also have 'unknown' and 'unknown_len'.
 *         // However, you must free 'unknown' when you're done.
 *         fate_mem_free(unknown);
 *     }
 * }
 * \endcode
 *
 * @{
 */



#ifndef FATE_PACKT_H
#define FATE_PACKT_H


#include <fate/defs.h>

struct fate_packt_objdesc {
    const char *name;
    const char *fields;
    unsigned id; /*!< For faster lookups */
};
typedef fate_packt_objdesc fate_packt_objdesc;

#define FATE_PACKT_GETDESC(type) type##_pkt

#define FATE_PACKT_ABLE_PROTO(type) \
    struct type; typedef struct type type; \
    extern fate_packt_desc FATE_PACKT_GETDESC(type)

#define FATE_PACKT_ABLE(type, fields) \
    struct type { fields }; \
    fate_packt_desc FATE_PACKT_GETDESC(type) = \
        { XSTRINGIFY(type), XSTRINGIFY(fields), 0}

struct fate_packt {
    /* TODO What here ? */
};
typedef struct fate_packt fate_packt;
void fate_packt_init(fate_packt *p, size_t size_hint);
void fate_packt_deinit(fate_packt *p);
void fate_packt_addobjv_real(fate_packt *p, fate_packt_objdesc *d,
                             void *objarray, size_t len);
void fate_packt_addobjvn_real(fate_packt *p, fate_packt_objdesc *d,
                              void **ptraddr, size_t *len);
#define fate_packt_addobjvn(packet, type, ary, len) \
        fate_packt_addobjvn_real(packet, &FATE_PACKT_GETDESC(type), ary, len)
#define fate_packt_addobjv(packet, type, ary, len) \
        fate_packt_addobjv_real(packet, &FATE_PACKT_GETDESC(type), ary, len)
#define fate_packt_addobj(packet, type, ary) \
        fate_packt_addobjv(packet, type, ary, 1)
void fate_packt_databuf(fate_packt *p, char **data, size_t *len);
void fate_packt_flags(fate_packt *p, unsigned long flags);
void fate_packt_pack(fate_packt *p);
void fate_packt_unpack(fate_packt *p);

/* @} */

#endif /* FATE_PACKT_H */
