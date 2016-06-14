#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "ipv6.h"
#include "udp6.h"
#include "tcp6.h"

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#ifndef IPPROTO_IPV6
#include <tpipv6.h>
#endif
#define FE_IPV6_SYSERR(code) WSA##code
#define FE_IPV6_GETLASTERROR() WSAGetLastError()
typedef int socklen_t;

#else

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/time.h>
#if defined(__linux__) && defined(_GNU_SOURCE)
#include <sys/sendfile.h>
#elif defined(__APPLE__) //darwin 9.0 (OSX 10.5)
#include <sys/uio.h>
#endif

#define FE_IPV6_SYSERR(code) code
#define FE_IPV6_GETLASTERROR() errno
#endif

const char *fe_ipv6_err_getstr(fe_ipv6_err err) {
    switch(err) {
    case FE_IPV6_ERR_UNKNOWN:
        return "Unknown error.";
    case FE_IPV6_ERR_NOMEM:
        return "Not enough memory.";
    case FE_IPV6_ERR_PEER_LOST:
        return "Connection reset by peer.";
    case FE_IPV6_ERR_INTERRUPTED:
        return "Interrupted by a signal.";
    case FE_IPV6_ERR_NOT_CONNECTED:
        return "The socket is not connected.";
    case FE_IPV6_ERR_WSSYSNOTREADY:
        return "The underlying network subsystem is not ready.";
    case FE_IPV6_ERR_WSVERSION:
        return "The winsock implementation does not support this version.";
    case FE_IPV6_ERR_DEVOUTQUEUEFULL:
        return "Device output queue was full, so the packet was discarded.";
    case FE_IPV6_ERR_NOINTERNET:
        return "Network unreachable.";
    case FE_IPV6_ERR_NOTREADY:
        return "Socket is not ready. Try again.";
    case FE_IPV6_ERR_SENDTOOLARGE:
        return "Datagram is bigger than an MTU (Maximum Transmission Unit) "
               "on the path and it cannot be fragmented: "
               "you should send smaller packets.";
    case FE_IPV6_ERR_NORECEIVER:
        return "No receiver was associated with the destination address.";
    case FE_IPV6_ERR_NOHOST:
        return "Host unreachable.";
    case FE_IPV6_ERR_NOIPV6:
        return "Your OS or hardware does not support dual-stack sockets.";
    case FE_IPV6_ERR_ALREADY_BOUND:
        return "The socket is already bound to a port.";
    case FE_IPV6_ERR_ADDR_IN_USE:
        return "The port is already taken.";
    case FE_IPV6_ERR_NOFILE:
        return "Could not open target file.";
    case FE_IPV6_ERR_NONE:
        return "No error.";
    default:
        return "There's no message for this error. This is a bug, please "
               "report to the maintainers.";
    }
}

static inline fe_ipv6_err fe_ipv6_systoerr(int err) {
#ifdef _WIN32
    if(err == WSATRY_AGAIN || err==WSAEWOULDBLOCK)
        return FE_IPV6_ERR_NOTREADY;
#else
    /* Duplicate case values on Linux. */
    if(err == EAGAIN || err == EWOULDBLOCK)
        return FE_IPV6_ERR_NOTREADY;
#endif
    switch(err) {
    case FE_IPV6_SYSERR(EAFNOSUPPORT):
        return FE_IPV6_ERR_NOIPV6;
    case FE_IPV6_SYSERR(EINPROGRESS):
        return FE_IPV6_ERR_NOTREADY;
    case FE_IPV6_SYSERR(EHOSTUNREACH):
        return FE_IPV6_ERR_NOHOST;
#ifdef _WIN32
    case WSAENETDOWN:
#endif
    case FE_IPV6_SYSERR(ENETUNREACH):
        return FE_IPV6_ERR_NOINTERNET;
    case FE_IPV6_SYSERR(EMSGSIZE):
        return FE_IPV6_ERR_SENDTOOLARGE;
    case FE_IPV6_SYSERR(ENOBUFS):
        return FE_IPV6_ERR_DEVOUTQUEUEFULL;
    case FE_IPV6_SYSERR(ECONNREFUSED):
        return FE_IPV6_ERR_NORECEIVER;
    case FE_IPV6_SYSERR(EADDRINUSE):
        return FE_IPV6_ERR_ADDR_IN_USE;
    case FE_IPV6_SYSERR(EFAULT):
#ifndef _WIN32
    case ENOMEM:
#endif
        return FE_IPV6_ERR_NOMEM;
    case FE_IPV6_SYSERR(ECONNRESET):
        return FE_IPV6_ERR_PEER_LOST;
    case FE_IPV6_SYSERR(ENOTCONN):
        return FE_IPV6_ERR_NOT_CONNECTED;
    case FE_IPV6_SYSERR(EINTR):
        return FE_IPV6_ERR_INTERRUPTED;
    default:
        return FE_IPV6_ERR_UNKNOWN;
    }
}


fe_ipv6_err fe_ipv6_setup(void) {
#ifdef _WIN32
    int err;
    WSADATA data;
    err = WSAStartup(MAKEWORD(2, 2), &data);
    if(err) {
        switch(err) {
        case WSASYSNOTREADY:
            return FE_IPV6_ERR_WSSYSNOTREADY;
        case WSAVERNOTSUPPORTED:
            return FE_IPV6_ERR_WSVERSION;
        }
        return FE_IPV6_ERR_UNKNOWN;
    }
    if(LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2) {
        WSACleanup();
        return FE_IPV6_ERR_WSVERSION; 
    }
#endif
    fe_udp6 sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if(sock==-1) {
        fe_ipv6_err ierr = fe_ipv6_systoerr(FE_IPV6_GETLASTERROR());
        fe_ipv6_cleanup();
        return ierr;
    }
    fe_udp6_deinit(&sock);
    return FE_IPV6_ERR_NONE;
}
void fe_ipv6_cleanup(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}

void fe_ipv6_get_public_addr(char addr[FE_IPV6_ADDRSTRLEN]) {
    fputs("TODO", stderr);
}
void fe_ipv6_get_local_addr(char addr[FE_IPV6_ADDRSTRLEN]) {
    fputs("TODO", stderr);
}

bool fe_udp6_init(fe_udp6 *s) {
    *s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if(*s==-1)
        return false;
#ifdef _WIN32
    DWORD val;
#else
    int val;
#endif
    /* val = IPV6_PMTUDISC_DO;
     * setsockopt(*s, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val, sizeof(val)); */
    val = false;
    setsockopt(*s, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&val, sizeof(val));
    return true;
}
void fe_udp6_deinit(fe_udp6 *s) {
#ifdef _WIN32
    closesocket(*s);
#else
    close(*s);
#endif
}

bool fe_tcp6_init(fe_tcp6 *s) {
    *s = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if(*s==-1)
        return false;
#ifdef _WIN32
    DWORD val;
#else
    int val;
#endif
    /* val = IPV6_PMTUDISC_DO;
     * setsockopt(*s, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val, sizeof(val)); */
    /* On Windows, dual-stack sockets are supported only on Vista and later. */
    val = false;
    setsockopt(*s, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&val, sizeof(val));
    return true;
}
void fe_tcp6_deinit(fe_tcp6 *s) {
    shutdown(*s, 
#ifdef _WIN32
        SD_BOTH
#else
        SHUT_RDWR
#endif
    );
    fe_udp6_deinit(s);
}
void fe_tcp6_recv_no_more(fe_tcp6 *s) {
    shutdown(*s, 
#ifdef _WIN32
        SD_RECEIVE
#else
        SHUT_RD
#endif
    );
}
void fe_tcp6_send_no_more(fe_tcp6 *s) {
    shutdown(*s, 
#ifdef _WIN32
        SD_SEND
#else
        SHUT_WR
#endif
    );
}

void fe_udp6_unblock(fe_udp6 *s) {
#ifdef _WIN32
    u_long val = true;
    ioctlsocket(*s, FIONBIO, &val);
#else
    int flags = fcntl(*s, F_GETFL, 0);
    if(flags < 0)
        return;
    fcntl(*s, F_SETFL, flags | O_NONBLOCK);
#endif
}
void fe_udp6_block(fe_udp6 *s) {
#ifdef _WIN32
    u_long val = false;
    ioctlsocket(*s, FIONBIO, &val);
#else
    int flags = fcntl(*s, F_GETFL, 0);
    if(flags < 0)
        return;
    fcntl(*s, F_SETFL, flags & ~O_NONBLOCK);
#endif
}
void fe_udp6_opt_priority(fe_udp6 *s, float prio) {
#if defined(__linux__)
    int val;
    //FIXME maybe 0 is the higest priority and 6 is lowest ?
    if(prio<0.f) val = 0;
    else if(prio > 1.f) val = 6;
    else val = prio*6.f;
    setsockopt(*s, SOL_SOCKET, SO_PRIORITY, (void*)&val, sizeof(val));
#endif
}
void fe_udp6_opt_recv_bufsize(fe_udp6 *s, size_t size) {
    const int val = size;
    setsockopt(*s, SOL_SOCKET, SO_RCVBUF, (void*)&val, sizeof(val));
}
void fe_udp6_opt_send_bufsize(fe_udp6 *s, size_t size) {
    const int val = size;
    setsockopt(*s, SOL_SOCKET, SO_SNDBUF, (void*)&val, sizeof(val));
}
static inline void ms_to_timeval(uint32_t ms, struct timeval *tv) {
    tv->tv_sec = ms/1000;
    tv->tv_usec = 1000*(ms%1000);
}
void fe_udp6_opt_send_timeout(fe_udp6 *s, uint32_t timeout_milliseconds) {
#ifdef _WIN32
    DWORD timeout = timeout_milliseconds;
#else
    struct timeval timeout;      
    ms_to_timeval(timeout_milliseconds, &timeout);
#endif
    setsockopt(*s, SOL_SOCKET, SO_SNDTIMEO, (void*) &timeout, sizeof(timeout));
}
void fe_udp6_opt_recv_timeout(fe_udp6 *s, uint32_t timeout_milliseconds) {
#ifdef _WIN32
    DWORD timeout = timeout_milliseconds;
#else
    struct timeval timeout;      
    ms_to_timeval(timeout_milliseconds, &timeout);
#endif
    setsockopt(*s, SOL_SOCKET, SO_RCVTIMEO, (void*) &timeout, sizeof(timeout));
}
int32_t fe_udp6_opt_get_mtu(fe_udp6 *s) {
#if defined(__linux__)
    int val;
    socklen_t val_len = sizeof(val);
    return getsockopt(*s, IPPROTO_IPV6, IPV6_MTU, (void*)&val, &val_len);
#else
    return FE_IPV6_DEFAULT_MTU;
#endif
}
void fe_udp6_opt_mtu(fe_udp6 *s, int32_t mtu) {
#if defined(__linux__)
    setsockopt(*s, IPPROTO_IPV6, IPV6_MTU, (void*)&mtu, sizeof(mtu));
#endif
}

void fe_ipv6_peer_from_udp6(fe_ipv6_peer *p, fe_udp6 *s) {
    socklen_t len = sizeof(*p);
    getsockname(*s, (void*) p, &len);
}
void fe_ipv6_peer_from_tcp6(fe_ipv6_peer *p, fe_tcp6 *s) {
    socklen_t len = sizeof(*p);
    getpeername(*s, (void*) p, &len);
}
void fe_ipv6_peer_from_localhost(fe_ipv6_peer *p, uint16_t port) {
    memset(p, 0, sizeof(*p));
    p->sin6_family = AF_INET6;
    p->sin6_addr = in6addr_loopback;
    p->sin6_port = htons(port);
}
void fe_ipv6_peer_from_addr(fe_ipv6_peer *p, const char addr_str[FE_IPV6_ADDRSTRLEN], uint16_t port) 
{
    memset(p, 0, sizeof(*p));
    p->sin6_family = AF_INET6;
    p->sin6_port = htons(port);
    inet_pton(AF_INET6, addr_str, &(p->sin6_addr));
}
bool fe_ipv6_check_addr(const char addr[FE_IPV6_ADDRSTRLEN]) {
    struct in6_addr tmp;
    return inet_pton(AF_INET6, addr, &tmp) > 0;
}
bool fe_ipv4_check_addr(const char addr[FE_IPV4_ADDRSTRLEN]) {
    struct in_addr tmp;
    return inet_pton(AF_INET, addr, &tmp) > 0;
}
void fe_ipv6_addr_from_ipv4(char addr[FE_IPV6_ADDRSTRLEN], 
                            const char addr4[FE_IPV4_ADDRSTRLEN]) {
    snprintf(addr, FE_IPV6_ADDRSTRLEN, "::ffff:%s", addr4);
}
bool fe_ipv6_peer_from_host(fe_ipv6_peer *p, const char host[FE_IPV6_HOSTMAXLEN], uint16_t port, 
                            bool udp) {
    struct addrinfo hints = {0};
    hints.ai_family = AF_INET6;
    hints.ai_socktype = udp ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol = udp ? IPPROTO_UDP : IPPROTO_TCP;
/* Maybe my MinGW is buggy, but I need this : */
#ifndef AI_NUMERICSERV
#define AI_NUMERICSERV 0x0008
#endif
#ifndef AI_ALL
#define AI_ALL         0x0100
#endif
#ifndef AI_V4MAPPED
#define AI_V4MAPPED    0x0800
#endif
    hints.ai_flags = AI_V4MAPPED | AI_ALL | AI_NUMERICSERV;
    char service[6];
    snprintf(service, 6, "%hu", port);
    struct addrinfo *info;
    int err = getaddrinfo(host, service, &hints, &info);
    if(err) {
        fprintf(stderr, "%s\n", gai_strerror(err));
        return false;
    }
    /* const struct addrinfo *cur;
       for(cur=info ; cur ; cur=cur->ai_next) ... */
    *p = *(fe_ipv6_peer*)info->ai_addr;
    freeaddrinfo(info);
    return true;
}

void fe_ipv6_peer_get_host(const fe_ipv6_peer *p, 
                           char host[FE_IPV6_HOSTMAXLEN],
                           bool udp) {
    int err = getnameinfo((void*) p, sizeof(*p), host, FE_IPV6_HOSTMAXLEN, 
                          NULL, 0, udp ? NI_DGRAM : 0);
    if(err) {
        fprintf(stderr, "%s\n", gai_strerror(err));
        *host = '\0';
    }
}
void fe_ipv6_peer_get_addr(const fe_ipv6_peer *p, char addr[FE_IPV6_ADDRSTRLEN]) {
    inet_ntop(AF_INET6, &(p->sin6_addr.s6_addr), addr, FE_IPV6_ADDRSTRLEN);
}
uint16_t fe_ipv6_peer_get_port(const fe_ipv6_peer *p) {
    return ntohs(p->sin6_port);
}

fe_ipv6_err fe_udp6_bind(fe_udp6 *s, uint16_t port) {
    fe_ipv6_peer p = {0};
    p.sin6_family = AF_INET6;
    p.sin6_addr = in6addr_any;
    p.sin6_port = htons(port);
    switch(bind(*s, (void*) &p, sizeof(p))) {
    case 0: return FE_IPV6_ERR_NONE;
    case FE_IPV6_SYSERR(EINVAL): return FE_IPV6_ERR_ALREADY_BOUND;
    }
    return fe_ipv6_systoerr(FE_IPV6_GETLASTERROR());
}


ssize_t fe_udp6_send(fe_udp6 *s, fe_ipv6_peer *p, const void *data, size_t len) {
    assert(p);
    ssize_t res = sendto(*s, data, len, 0, (void*) p, sizeof(*p));
    return res>=0 ? res : fe_ipv6_systoerr(FE_IPV6_GETLASTERROR());
}
ssize_t fe_udp6_reply(fe_udp6 *s, fe_ipv6_peer *p, const void *data, size_t len) {
#if defined(__linux__)
    assert(p);
    ssize_t res = sendto(*s, data, len, MSG_CONFIRM, (void*) p, sizeof(*p));
    return res>=0 ? res : fe_ipv6_systoerr(FE_IPV6_GETLASTERROR());
#else
    return fe_udp6_send(s, p, data, len);
#endif
}
ssize_t fe_udp6_recv(fe_udp6 *s, fe_ipv6_peer *p, void *data, size_t len) {
    socklen_t addrlen = sizeof(*p);
    ssize_t res;
    res = recvfrom(*s, data, len, 0, (void*) p, p ? &addrlen : NULL);
    return res>=0 ? res : fe_ipv6_systoerr(FE_IPV6_GETLASTERROR());
}
ssize_t fe_udp6_peek(fe_udp6 *s, fe_ipv6_peer *p, void *data, size_t len) {
    socklen_t addrlen = sizeof(*p);
    ssize_t res;
    res = recvfrom(*s, data, len, MSG_PEEK, (void*) p, p ? &addrlen : NULL);
    return res>=0 ? res : fe_ipv6_systoerr(FE_IPV6_GETLASTERROR());
}

ssize_t fe_tcp6_send(fe_tcp6 *s, const void * data, size_t len) {
    ssize_t res = send(*s, data, len, 0);
    return res>=0 ? res : fe_ipv6_systoerr(FE_IPV6_GETLASTERROR());
}
ssize_t fe_tcp6_recv(fe_tcp6 *s, void *data, size_t len) {
    ssize_t res = recv(*s, data, len, 0);
    return res>=0 ? res : fe_ipv6_systoerr(FE_IPV6_GETLASTERROR());
}
ssize_t fe_tcp6_peek(fe_tcp6 *s, void *data, size_t len) {
    ssize_t res = recv(*s, data, len, MSG_PEEK);
    return res>=0 ? res : fe_ipv6_systoerr(FE_IPV6_GETLASTERROR());
}
ssize_t fe_tcp6_sendfile(fe_tcp6 *s, const char *filename) {
#if (defined(__linux__) && defined(_GNU_SOURCE)) || defined(__APPLE__)
    int fd = open(filename, O_RDONLY, 0666);
    if(fd < 0)
        return FE_IPV6_ERR_NOFILE;
    off_t size = lseek(fd, 0, SEEK_END);
    uint32_t size_nbo = htonl(size);
    fe_tcp6_send(s, &size_nbo, sizeof(uint32_t));
    #if defined(__APPLE__)
        off_t offset, sent; /* Read to EOF */
        for(offset=sent=0 ; ; offset+=sent, sent=0) {
            int err = sendfile(fd, *s, offset, &sent, NULL, 0);
            if(err >= 0) {
                sent += offset;
                break;
            }
            if(errno != EAGAIN) {
                sent = fe_ipv6_systoerr(errno);
                break;
            }
        }
    #elif defined(__linux__) && defined(_GNU_SOURCE)
        lseek(fd, 0, SEEK_SET);
        ssize_t sent = splice(fd, NULL, *s, NULL, size, 0);
        if(sent < 0)
            sent = fe_ipv6_systoerr(errno);
    #endif
    close(fd);
    return sent;
#else
    fputs("Sorry, sendfile() is NIY.\n", stderr);
    return FE_IPV6_ERR_UNKNOWN;
#endif
}
ssize_t fe_tcp6_recvfile(fe_tcp6 *s, const char *file) {
#if defined(__linux__) && defined(_GNU_SOURCE)
    uint32_t size;
    fe_tcp6_recv(s, &size, sizeof(uint32_t));
    size = ntohl(size);
    int fd = creat(file, 0666);
    if(fd < 0)
        return 0;
    splice(*s, NULL, fd, NULL, size, 0);
    close(fd);
#else
    fputs("Sorry, recvfile() is NIY.\n", stderr);
    return FE_IPV6_ERR_UNKNOWN;
#endif
}


fe_ipv6_err fe_tcp6_listen(fe_tcp6 *s, int32_t max_requests) {
    return listen(*s, max_requests)
        ? fe_ipv6_systoerr(FE_IPV6_GETLASTERROR()) : FE_IPV6_ERR_NONE;
}
fe_ipv6_err fe_tcp6_accept(fe_tcp6 *s, fe_tcp6 *out_sock, fe_ipv6_peer *peer)
{
    socklen_t addrlen = sizeof(*peer);
    int res = accept(*s, (void*) peer, &addrlen);
    return res>=0 ? *out_sock=res, FE_IPV6_ERR_NONE 
           : fe_ipv6_systoerr(FE_IPV6_GETLASTERROR());
}
fe_ipv6_err fe_tcp6_connect(fe_tcp6 *s, const fe_ipv6_peer *peer) {
    return connect(*s, (const void*)peer, sizeof(*peer)) 
        ? fe_ipv6_systoerr(FE_IPV6_GETLASTERROR()) : FE_IPV6_ERR_NONE;
}


struct fe_ipv6_hub {
    int nfds;
    fd_set read_set;
    fd_set ready_set;
};
void fe_ipv6_hub_init(fe_ipv6_hub *s) {
    s->nfds = 0;
    FD_ZERO(&s->read_set);
    FD_ZERO(&s->ready_set);
}
void fe_ipv6_hub_deinit(fe_ipv6_hub *s) {}
bool fe_ipv6_hub_add_udp6(fe_ipv6_hub *s, fe_udp6 *sock) {
    if(*sock >= FD_SETSIZE)
        return false;
    if(FD_ISSET(*sock, &s->read_set))
        return true;
    FD_SET(*sock, &s->read_set);
    if((*sock)+1 > s->nfds)
        s->nfds = (*sock)+1;
    return true;
}
void fe_ipv6_hub_rm_udp6(fe_ipv6_hub *s, fe_udp6 *sock) {
    if(!FD_ISSET(*sock, &s->read_set))
        return;
    FD_CLR(*sock, &s->read_set);
    FD_CLR(*sock, &s->ready_set);
}
bool fe_ipv6_hub_wait(fe_ipv6_hub *s, uint32_t timeout_msec) {
    struct timeval timeout;      
    ms_to_timeval(timeout_msec, &timeout);
    s->ready_set = s->read_set;
    return select(s->nfds, &s->ready_set, NULL, NULL, 
                  timeout_msec==~0 ? NULL : &timeout) > 0;
}
bool fe_ipv6_hub_test_udp6(fe_ipv6_hub *s, fe_udp6 *sock) {
    return FD_ISSET(*sock, &s->ready_set);
}

