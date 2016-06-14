#ifndef FE_IPV6_H
#define FE_IPV6_H

#include <stdint.h>
#include <stdbool.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET fe_udp6;
typedef SOCKET fe_tcp6;
#else
#include <arpa/inet.h>
typedef int fe_udp6;
typedef int fe_tcp6;
#endif
typedef struct sockaddr_in6 fe_ipv6_peer;


/* This API supports IPv6, and IPv4 through remapping. 
 * According to Apple, its is important to use the POSIX networking API
 * in a separate thread. 
 * Here, the caller is responsible for ensuring that. */

#define FE_IPV6_HOSTMAXLEN 1025 /* NI_MAXHOST */
#define FE_IPV6_ADDRSTRLEN 48 /* INET6_ADDRSTRLEN */
#define FE_IPV4_ADDRSTRLEN 16
#define FE_IPV6_RECVBUFMINSIZE 128
#define FE_IPV6_SENDBUFMINSIZE 1024
#define FE_IPV6_DEFAULT_MTU 1500

enum fe_ipv6_err {
    FE_IPV6_ERR_UNKNOWN = -64,
    FE_IPV6_ERR_NOMEM,
    FE_IPV6_ERR_PEER_LOST,
    FE_IPV6_ERR_INTERRUPTED,
    FE_IPV6_ERR_NOT_CONNECTED,
    FE_IPV6_ERR_WSSYSNOTREADY,
    FE_IPV6_ERR_WSVERSION,
    FE_IPV6_ERR_DEVOUTQUEUEFULL,
    FE_IPV6_ERR_NOINTERNET,
    FE_IPV6_ERR_NOTREADY,
    FE_IPV6_ERR_SENDTOOLARGE,
    FE_IPV6_ERR_NORECEIVER,
    FE_IPV6_ERR_NOHOST,
    FE_IPV6_ERR_NOIPV6,
    FE_IPV6_ERR_ALREADY_BOUND,
    FE_IPV6_ERR_ADDR_IN_USE,
    FE_IPV6_ERR_NOFILE,
    FE_IPV6_ERR_NONE = 0
};
typedef enum fe_ipv6_err fe_ipv6_err;
const char *fe_ipv6_err_getstr(fe_ipv6_err err);

fe_ipv6_err fe_ipv6_setup(void);
void     fe_ipv6_cleanup(void);
void     fe_ipv6_get_public_addr(char addr[FE_IPV6_ADDRSTRLEN]);
void     fe_ipv6_get_local_addr(char addr[FE_IPV6_ADDRSTRLEN]);
bool     fe_ipv6_check_addr(const char addr[FE_IPV6_ADDRSTRLEN]);
bool     fe_ipv4_check_addr(const char addr[FE_IPV4_ADDRSTRLEN]);
void     fe_ipv6_addr_from_ipv4(char addr[FE_IPV6_ADDRSTRLEN], const char addr4[FE_IPV4_ADDRSTRLEN]);
void     fe_ipv6_peer_from_udp6(fe_ipv6_peer *p, fe_udp6 *s);
void     fe_ipv6_peer_from_tcp6(fe_ipv6_peer *p, fe_tcp6 *s);
void     fe_ipv6_peer_from_tcp6_self(fe_ipv6_peer *p, fe_tcp6 *s);
void     fe_ipv6_peer_from_localhost(fe_ipv6_peer *p, uint16_t port);
void     fe_ipv6_peer_from_addr(fe_ipv6_peer *p, const char addr_str[FE_IPV6_ADDRSTRLEN], uint16_t port);
bool     fe_ipv6_peer_from_udp6_host(fe_ipv6_peer *p, const char host[FE_IPV6_HOSTMAXLEN], uint16_t port);
bool     fe_ipv6_peer_from_tcp6_host(fe_ipv6_peer *p, const char host[FE_IPV6_HOSTMAXLEN], uint16_t port);
/* Be aware that this function may stall for a few seconds. 
 * On Windows it might even print a message to the console in your 
 * locale without asking. */
void     fe_ipv6_peer_get_udp6_host(const fe_ipv6_peer *p, char host[FE_IPV6_HOSTMAXLEN]);
void     fe_ipv6_peer_get_tcp6_host(const fe_ipv6_peer *p, char host[FE_IPV6_HOSTMAXLEN]);
void     fe_ipv6_peer_get_addr(const fe_ipv6_peer *p, char addr[FE_IPV6_ADDRSTRLEN]);
uint16_t fe_ipv6_peer_get_port(const fe_ipv6_peer *p);

typedef struct fe_ipv6_hub fe_ipv6_hub;
void fe_ipv6_hub_init(fe_ipv6_hub *s);
void fe_ipv6_hub_deinit(fe_ipv6_hub *s);
bool fe_ipv6_hub_add_udp6(fe_ipv6_hub *s, fe_udp6 *sock);
bool fe_ipv6_hub_add_tcp6(fe_ipv6_hub *s, fe_tcp6 *sock);
void fe_ipv6_hub_rm_udp6(fe_ipv6_hub *s, fe_udp6 *sock);
void fe_ipv6_hub_rm_tcp6(fe_ipv6_hub *s, fe_tcp6 *sock);
bool fe_ipv6_hub_wait(fe_ipv6_hub *s, uint32_t timeout_msec);
bool fe_ipv6_hub_test_udp6(fe_ipv6_hub *s, fe_udp6 *sock);
bool fe_ipv6_hub_test_tcp6(fe_ipv6_hub *s, fe_tcp6 *sock);


/* End of API */
#define fe_ipv6_peer_from_tcp6_self fe_ipv6_peer_from_udp6
bool fe_ipv6_peer_from_host(fe_ipv6_peer *p, const char host[FE_IPV6_HOSTMAXLEN], uint16_t port, 
                            bool udp);
#define fe_ipv6_peer_from_udp6_host(p,host,port) \
        fe_ipv6_peer_from_host(p, host, port, true)
#define fe_ipv6_peer_from_tcp6_host(p,host,port) \
        fe_ipv6_peer_from_host(p, host, port, false)
void fe_ipv6_peer_get_host(const fe_ipv6_peer *p, 
                           char host[FE_IPV6_HOSTMAXLEN],
                           bool udp);
#define fe_ipv6_peer_get_udp6_host(p, host) \
        fe_ipv6_peer_get_host(p, host, true)
#define fe_ipv6_peer_get_tcp6_host(p, host) \
        fe_ipv6_peer_get_host(p, host, false)

#define fe_ipv6_hub_add_tcp6 fe_ipv6_hub_add_udp6
#define fe_ipv6_hub_rm_tcp6 fe_ipv6_hub_rm_udp6
#define fe_ipv6_hub_test_tcp6 fe_ipv6_hub_test_udp6
#endif /* FE_IPV6_H */
