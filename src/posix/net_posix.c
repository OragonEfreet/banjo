#include <banjo/assert.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/net.h>

#include "check.h"
#include "net_layer.h"

#include <arpa/inet.h>  // inet_ntop
#include <errno.h> // errno
#include <netdb.h>     // addrinfo
#include <stdio.h>     // snprintf
#include <string.h> //strerror
#include <sys/socket.h> 
#include <sys/types.h> 
/* #include <ifaddrs.h>   // getifaddrs, freeifaddrs */
/* #include <net/if.h>    // IFF_UP, IFF_LOOPBACK */

#define QUEUE_SIZE 5

struct bj_tcp_listener {
    int socket;
};

void bj_begin_network(void) {
    // EMPTY
}

void bj_end_network(void) {
    // EMPTY
}

/* void bj_unbind( */
/*     struct bj_tcp_listener* listener */
/* ) { */
/*     bj_check(listener); */
/*     close(listener->socket); */
/*     bj_free(listener); */
/* } */

struct bj_tcp_listener* bj_listen_tcp(
    const struct bj_net_addr* listen_addr,
    uint16_t                  port,
    struct bj_error**         error
) {
    bj_assert(listen_addr == 0);

    (void)listen_addr;
    (void)error;
    (void)port;

    static char str_port[6];
    const int written = snprintf(str_port, sizeof(str_port), "%u", port);
    if(written <= 0 || written >= (int)sizeof(str_port)) {
        bj_set_error_fmt(
            error, BJ_ERROR_NETWORK, 
            "incorrect port number '%u'", port
        );
        return 0;
    }

    const struct addrinfo hints = {
        .ai_family   = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_flags    = AI_PASSIVE,
    };
    struct addrinfo* listen_addrs = 0;
    
    const int gaierror = getaddrinfo(0, str_port, &hints, &listen_addrs);
    if(gaierror) {
        bj_set_error(error, BJ_ERROR_NETWORK, gai_strerror(gaierror));
        return 0;
    }

    for(
        struct addrinfo* addr = listen_addrs;
        addr != 0;
        addr = addr->ai_next
    ) {
        const int sockfd = socket(
            addr->ai_family, addr->ai_socktype, addr->ai_protocol
        );

        if(sockfd == -1) {
            bj_set_error(error, BJ_ERROR_NETWORK_SOCKET, strerror(errno));
            freeaddrinfo(listen_addrs);
            return 0;
        }

        if(bind(sockfd, addr->ai_addr, addr->ai_addrlen) == -1) {
            bj_set_error(error, BJ_ERROR_NETWORK_SOCKET, strerror(errno));
            close(sockfd);
            freeaddrinfo(listen_addrs);
            return 0;
        }

        if(listen(sockfd, BJ_NET_LISTEN_QUEUE) == -1) {
            bj_set_error(error, BJ_ERROR_NETWORK_SOCKET, strerror(errno));
            close(sockfd);
            freeaddrinfo(listen_addrs);
            return 0;
        }

        struct bj_tcp_listener* listener = bj_calloc(sizeof(struct bj_tcp_listener));
        listener->socket = sockfd;
        return listener;
    }

    freeaddrinfo(listen_addrs);
    bj_set_error(error, BJ_ERROR_NOT_IMPLEMENTED, "not implemented");
    return 0;
}

BANJO_EXPORT struct bj_tcp_stream* bj_accept_tcp(
    struct bj_tcp_listener* listener,
    struct bj_error**         error
) {
    bj_check_or_0(listener);
    (void)error;

    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);
    int new_fd = accept(listener->socket, (struct sockaddr*)&their_addr, &addr_size);
    if(new_fd == -1) {
        bj_set_error(error, BJ_ERROR_NETWORK_SOCKET, strerror(errno));
        return 0;
    }

    return 0;
}

int bj_tcp_recv(
    struct bj_tcp_stream* stream,
    void*                 buf,
    size_t                len
) {
    (void)stream;
    (void)buf;
    (void)len;
    return 0;
}

int bj_tcp_send(
    struct bj_tcp_stream* stream,
    const void*           buf,
    size_t                len
) {
    (void)stream;
    (void)buf;
    (void)len;
    return 0;
}

void bj_close_tcp_stream(
    struct bj_tcp_stream* stream
) {
    (void)stream;
}

void bj_close_tcp_listener(
    struct bj_tcp_listener* listener
) {
    (void)listener;

}
