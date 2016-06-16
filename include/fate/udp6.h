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

/*! \file fate/udp6.h
 *  \brief The UDP sockets module.
 *  \defgroup udp6 UDP6 sockets
 *
 * @{
 */



#ifndef FE_UDP6_H
#define FE_UDP6_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <fate/defs.h>
#include <fate/ipv6.h>

/*! \brief TODO*/
#define FE_UDP6_DGRAMMAXSIZE 65507

/*! \brief TODO*/
typedef ssize_t (*fe_udp6_sendfunc)(fe_udp6 *s, fe_ipv6_peer *p,const void*data,size_t len);

/*! \brief TODO*/
bool     fe_udp6_init(fe_udp6 *s);
/*! \brief TODO*/
void     fe_udp6_deinit(fe_udp6 *s);
/*! \brief TODO*/
void     fe_udp6_unblock(fe_udp6 *s);
/*! \brief TODO*/
void     fe_udp6_block(fe_udp6 *s);
/*! \brief TODO*/
void     fe_udp6_opt_priority(fe_udp6 *s, float prio);
/*! \brief TODO*/
void     fe_udp6_opt_recv_bufsize(fe_udp6 *s, size_t size);
/*! \brief TODO*/
void     fe_udp6_opt_send_bufsize(fe_udp6 *s, size_t size);
/*! \brief TODO*/
void     fe_udp6_opt_ttl(fe_udp6 *s, size_t size);
/*! \brief TODO*/
void     fe_udp6_opt_get_ttl(fe_udp6 *s, size_t size);
/*! \brief TODO*/
void     fe_udp6_opt_recv_timeout(fe_udp6 *s, uint32_t timeout_msec);
/*! \brief TODO*/
void     fe_udp6_opt_send_timeout(fe_udp6 *s, uint32_t timeout_msec);
/*! \brief TODO*/
int32_t  fe_udp6_opt_get_mtu(fe_udp6 *s);
/*! \brief TODO*/
void     fe_udp6_opt_mtu(fe_udp6 *s, int32_t mtu);
/*! \brief TODO*/
fe_ipv6_err fe_udp6_bind(fe_udp6 *s, uint16_t port);
/*! \brief TODO*/
ssize_t fe_udp6_send(fe_udp6 *s, fe_ipv6_peer *p,const void*data,size_t len);
/*! \brief TODO*/
ssize_t fe_udp6_reply(fe_udp6 *s, fe_ipv6_peer *p,const void*data,size_t len);
/*! \brief TODO*/
ssize_t fe_udp6_recv(fe_udp6 *s, fe_ipv6_peer *p, void *data, size_t len);
/*! \brief TODO*/
ssize_t fe_udp6_peek(fe_udp6 *s, fe_ipv6_peer *p, void *data, size_t len);

/*! @} */
#endif /* FE_UDP6_H */
