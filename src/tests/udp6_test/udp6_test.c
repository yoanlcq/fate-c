#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fate/defs.h>
#include <fate/ipv6.h>
#include <fate/udp6.h>

/* Windows: -lws2_32 */

static const uint16_t PORT = 5001;

char buf[5400];

void print_peer(const fe_ipv6_peer *peer) {
    char host[FE_IPV6_HOSTMAXLEN];
    char addr[FE_IPV6_ADDRSTRLEN];
    fe_ipv6_peer_get_udp6_host(peer, host);
    fe_ipv6_peer_get_addr(peer, addr);
    uint16_t port = fe_ipv6_peer_get_port(peer);
    printf("{hostname: \"%s\", addr: \"%s\", port: %hu}", 
            host, addr, port);
}

void server(void) {
    fe_udp6 sock;
    if(!fe_udp6_init(&sock)) {
        fprintf(stderr, "Failed to create IPv6 UDP socket.\n");
        return;
    }
    fe_udp6_block(&sock);
    fe_ipv6_err err = fe_udp6_bind(&sock, PORT);
    if(err < 0) {
        fprintf(stderr, "Failed to bind to port %hu:\n\t%s\n", 
                PORT, fe_ipv6_err_getstr(err));
        return;
    }
    fe_ipv6_peer peer;
    fe_ipv6_peer_from_udp6(&peer, &sock);
    printf("You are the server : ");
    print_peer(&peer);
    putchar('\n');

    for(;;) {
        ssize_t recvd = fe_udp6_recv(&sock, &peer, buf, sizeof(buf));
        if(recvd < 0) {
            fprintf(stderr, "recv error : %s\n", fe_ipv6_err_getstr(recvd));
            continue;
        }
        printf("Received %" PRIssize_t " bytes of data from ", recvd);
        print_peer(&peer);
        printf(" :\n\t");
        fwrite(buf, 1, recvd, stdout);
        putchar('\n');
    }
    fe_udp6_deinit(&sock);
}

void client(const char *addr) {
    fe_udp6 sock;
    if(!fe_udp6_init(&sock)) {
        fprintf(stderr, "Failed to create IPv6 UDP socket.\n");
        return;
    }
    fe_udp6_block(&sock);
    fe_udp6_sendfunc sendfunc = fe_udp6_send;
    fe_ipv6_peer peer;
    fe_ipv6_peer_from_udp6(&peer, &sock);
    printf("You are the client : ");
    print_peer(&peer);
    putchar('\n');
    fe_ipv6_peer_from_addr(&peer, addr, PORT);

    for(;;) {
        printf("Your input ([Enter] to send) :\n> ");
        fflush(stdout);
        if(!fgets(buf, sizeof(buf), stdin))
            continue;
        ssize_t sent = sendfunc(&sock, &peer, buf, strlen(buf)-1);
        if(sent < 0) {
            fprintf(stderr, "send error : %s\n", fe_ipv6_err_getstr(sent));
            continue;
        }
        printf("Sent %" PRIssize_t " bytes of data to ", sent);
        print_peer(&peer);
        putchar('\n');
    }
    fe_udp6_deinit(&sock);
}

int main(int argc, char *argv[]) {
    fe_ipv6_err err = fe_ipv6_setup();
    if(err) {
        fprintf(stderr, "Fatal: %s\n", fe_ipv6_err_getstr(err));
        return 1;
    }
    if(argc <= 1)
        server();
    else {
        if(fe_ipv6_check_addr(argv[1]))
            client(argv[1]);
        else if(fe_ipv4_check_addr(argv[1])) {
            char addr[FE_IPV6_ADDRSTRLEN];
            fe_ipv6_addr_from_ipv4(addr, argv[1]);
            client(addr);
        } else {
            fputs("Invalid address format in argv[1]!\n", stderr);
            client("::1");
        }
    }
    fe_ipv6_cleanup();
    return 0;
}
