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

/*! \file fate/tcp6.h
 *  \brief The TCP sockets module.
 *  \defgroup tcp6 TCP6 sockets
 *
 * @{
 */



#ifndef FE_TCP6_H
#define FE_TCP6_H

#include <stdint.h>
#include <stdbool.h>

#include <fate/ipv6.h>

/*! \brief TODO*/
bool     fe_tcp6_init(fe_tcp6 *s);
/*! \brief TODO*/
void     fe_tcp6_deinit(fe_tcp6 *s);
/*! \brief TODO*/
void     fe_tcp6_recv_no_more(fe_tcp6 *s);
/*! \brief TODO*/
void     fe_tcp6_send_no_more(fe_tcp6 *s);
/*! \brief TODO*/
void     fe_tcp6_unblock(fe_tcp6 *s);
/*! \brief TODO*/
void     fe_tcp6_block(fe_tcp6 *s);
/*! \brief TODO*/
void     fe_tcp6_opt_priority(fe_tcp6 *s, float prio);
/*! \brief TODO*/
void     fe_tcp6_opt_recv_bufsize(fe_tcp6 *s, size_t size);
/*! \brief TODO*/
void     fe_tcp6_opt_send_bufsize(fe_tcp6 *s, size_t size);
/*! \brief TODO*/
void     fe_tcp6_opt_ttl(fe_tcp6 *s, size_t size);
/*! \brief TODO*/
void     fe_tcp6_opt_get_ttl(fe_tcp6 *s, size_t size);
/*! \brief TODO*/
void     fe_tcp6_opt_recv_timeout(fe_tcp6 *s, uint32_t timeout_msec);
/*! \brief TODO*/
void     fe_tcp6_opt_send_timeout(fe_tcp6 *s, uint32_t timeout_msec);
/*! \brief TODO*/
int32_t  fe_tcp6_opt_get_mtu(fe_tcp6 *s);
/*! \brief TODO*/
void     fe_tcp6_opt_mtu(fe_tcp6 *s, int32_t mtu);
/*! \brief TODO*/
fe_ipv6_err fe_tcp6_bind(fe_tcp6 *s, uint16_t port);
/*! \brief TODO*/
fe_ipv6_err fe_tcp6_listen(fe_tcp6 *s, int32_t max_requests);
/*! \brief TODO*/
fe_ipv6_err fe_tcp6_accept(fe_tcp6 *s, fe_tcp6 *out_sock, fe_ipv6_peer *peer);
/*! \brief TODO*/
fe_ipv6_err fe_tcp6_connect(fe_tcp6 *s, const fe_ipv6_peer *peer);
/*! \brief TODO*/
ssize_t fe_tcp6_send(fe_tcp6 *s, const void * data, size_t len);
/*! \brief TODO*/
ssize_t fe_tcp6_recv(fe_tcp6 *s, void *data, size_t len);
/*! \brief TODO*/
ssize_t fe_tcp6_peek(fe_tcp6 *s, void *data, size_t len);
/* Uses sendfile() or splice() system calls if available. */
/*! \brief TODO*/
ssize_t fe_tcp6_sendfile(fe_tcp6 *s, const char *file);
/*! \brief TODO*/
ssize_t fe_tcp6_recvfile(fe_tcp6 *s, const char *file);

/* End of API */
#include <fate/udp6.h>
#define fe_tcp6_unblock fe_udp6_unblock
#define fe_tcp6_block fe_udp6_block
#define fe_tcp6_opt_priority fe_udp6_opt_priority
#define fe_tcp6_opt_recv_bufsize fe_udp6_opt_recv_bufsize
#define fe_tcp6_opt_send_bufsize fe_udp6_opt_send_bufsize
#define fe_tcp6_opt_ttl fe_udp6_opt_ttl
#define fe_tcp6_opt_get_ttl fe_udp6_opt_get_ttl
#define fe_tcp6_opt_recv_timeout fe_udp6_opt_recv_timeout
#define fe_tcp6_opt_send_timeout fe_udp6_opt_send_timeout
#define fe_tcp6_opt_get_mtu fe_udp6_opt_get_mtu
#define fe_tcp6_opt_mtu fe_udp6_opt_mtu
#define fe_tcp6_bind fe_udp6_bind

/*! @} */
#endif /* FE_TCP6_H */
