#ifndef FE_TCP6_H
#define FE_TCP6_H

#include <stdint.h>
#include <stdbool.h>

#include "ipv6.h"

bool     fe_tcp6_init(fe_tcp6 *s);
void     fe_tcp6_deinit(fe_tcp6 *s);
void     fe_tcp6_recv_no_more(fe_tcp6 *s);
void     fe_tcp6_send_no_more(fe_tcp6 *s);
void     fe_tcp6_unblock(fe_tcp6 *s);
void     fe_tcp6_block(fe_tcp6 *s);
void     fe_tcp6_opt_priority(fe_tcp6 *s, float prio);
void     fe_tcp6_opt_recv_bufsize(fe_tcp6 *s, size_t size);
void     fe_tcp6_opt_send_bufsize(fe_tcp6 *s, size_t size);
void     fe_tcp6_opt_ttl(fe_tcp6 *s, size_t size);
void     fe_tcp6_opt_get_ttl(fe_tcp6 *s, size_t size);
void     fe_tcp6_opt_recv_timeout(fe_tcp6 *s, uint32_t timeout_msec);
void     fe_tcp6_opt_send_timeout(fe_tcp6 *s, uint32_t timeout_msec);
int32_t  fe_tcp6_opt_get_mtu(fe_tcp6 *s);
void     fe_tcp6_opt_mtu(fe_tcp6 *s, int32_t mtu);
fe_ipv6_err fe_tcp6_bind(fe_tcp6 *s, uint16_t port);
fe_ipv6_err fe_tcp6_listen(fe_tcp6 *s, int32_t max_requests);
fe_ipv6_err fe_tcp6_accept(fe_tcp6 *s, fe_tcp6 *out_sock, fe_ipv6_peer *peer);
fe_ipv6_err fe_tcp6_connect(fe_tcp6 *s, const fe_ipv6_peer *peer);
ssize_t fe_tcp6_send(fe_tcp6 *s, const void * data, size_t len);
ssize_t fe_tcp6_recv(fe_tcp6 *s, void *data, size_t len);
ssize_t fe_tcp6_peek(fe_tcp6 *s, void *data, size_t len);
/* Uses sendfile() or splice() system calls if available. */
/* ssize_t fe_tcp6_sendfile(fe_tcp6 *s, const char *file); */
/* ssize_t fe_tcp6_recvfile(fe_tcp6 *s, const char *file); */

/* End of API */
#include "udp6.h"
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

#endif /* FE_TCP6_H */
