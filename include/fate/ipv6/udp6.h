#ifndef FE_UDP6_H
#define FE_UDP6_H

#include <stdint.h>
#include <stdbool.h>

#include "ipv6.h"

#define FE_UDP6_DGRAMMAXSIZE 65507

typedef ssize_t (*fe_udp6_sendfunc)(fe_udp6 *s, fe_ipv6_peer *p,const void*data,size_t len);

bool     fe_udp6_init(fe_udp6 *s);
void     fe_udp6_deinit(fe_udp6 *s);
void     fe_udp6_unblock(fe_udp6 *s);
void     fe_udp6_block(fe_udp6 *s);
void     fe_udp6_opt_priority(fe_udp6 *s, float prio);
void     fe_udp6_opt_recv_bufsize(fe_udp6 *s, size_t size);
void     fe_udp6_opt_send_bufsize(fe_udp6 *s, size_t size);
void     fe_udp6_opt_ttl(fe_udp6 *s, size_t size);
void     fe_udp6_opt_get_ttl(fe_udp6 *s, size_t size);
void     fe_udp6_opt_recv_timeout(fe_udp6 *s, uint32_t timeout_msec);
void     fe_udp6_opt_send_timeout(fe_udp6 *s, uint32_t timeout_msec);
int32_t  fe_udp6_opt_get_mtu(fe_udp6 *s);
void     fe_udp6_opt_mtu(fe_udp6 *s, int32_t mtu);
fe_ipv6_err fe_udp6_bind(fe_udp6 *s, uint16_t port);
ssize_t fe_udp6_send(fe_udp6 *s, fe_ipv6_peer *p,const void*data,size_t len);
ssize_t fe_udp6_reply(fe_udp6 *s, fe_ipv6_peer *p,const void*data,size_t len);
ssize_t fe_udp6_recv(fe_udp6 *s, fe_ipv6_peer *p, void *data, size_t len);
ssize_t fe_udp6_peek(fe_udp6 *s, fe_ipv6_peer *p, void *data, size_t len);

#endif /* FE_UDP6_H */
