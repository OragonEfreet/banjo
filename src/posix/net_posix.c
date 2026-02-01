#include <banjo/assert.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/net.h>

#include "check.h"

#include <arpa/inet.h>  // inet_ntop
/* #include <ifaddrs.h>   // getifaddrs, freeifaddrs */
/* #include <net/if.h>    // IFF_UP, IFF_LOOPBACK */
#include <errno.h>
#include <netdb.h>     // addrinfo
#include <stdio.h>     // snprintf

struct bj_tcp_listener {
    int socket;
};

void bj_begin_network(void) {
    // EMPTY
}

void bj_end_network(void) {
    // EMPTY
}

void bj_unbind(
    struct bj_tcp_listener* listener
) {
    bj_check(listener);
    close(listener->socket);
    bj_free(listener);
}

static struct bj_tcp_listener* bj_bind_addrinfo(
    struct addrinfo* bindaddr,
    uint16_t         backlog
) {
    // Create socket
    const int listen_socket = socket(
        bindaddr->ai_family,
        bindaddr->ai_socktype,
        bindaddr->ai_protocol
    );
    if(listen_socket <= 0) {
        bj_err("socket() failed: %u", errno);
        return 0;
    }

    setsockopt(listen_socket, 
        IPPROTO_IPV6, IPV6_V6ONLY,
        &(int){0}, sizeof(int)
    );

    if(bind(listen_socket, bindaddr->ai_addr, bindaddr->ai_addrlen)) {
        bj_err("bind() failed: %u", errno);
        close(listen_socket);
        return 0;
    }

    if(listen(listen_socket, backlog) < 0) {
        bj_err("listen() failed: %u", errno);
        close(listen_socket);
        return 0;
    }

    struct bj_tcp_listener* listener = bj_calloc(sizeof(struct bj_tcp_listener));
    listener->socket = listen_socket;
    return listener;
}


struct bj_tcp_listener* bj_bind(
    const struct bj_net_addr* addr,
    uint16_t                  port,
    uint16_t                  queue_size
) {
    bj_assert(addr == 0);

    char service[6];
    snprintf(service, sizeof(service), "%u", port);

    const struct addrinfo hints = {
        .ai_family   = AF_INET6,
        .ai_socktype = SOCK_STREAM,
        .ai_flags    = AI_PASSIVE,
    };

    struct addrinfo *bindaddrs = 0;

    const int res = getaddrinfo(0, service, &hints, &bindaddrs);
    if(res) {
        bj_err("cannot configure local address: %s", gai_strerror(res));
        return 0;
    }

    struct bj_tcp_listener* listener = 0;
    struct addrinfo* bindaddr        = bindaddrs;
    while(bindaddr) {
        listener = bj_bind_addrinfo(bindaddr, queue_size);
        if(listener > 0) {
            break;
        }
        bindaddr = bindaddr->ai_next;
    }

    freeaddrinfo(bindaddrs);

    return listener;
}
