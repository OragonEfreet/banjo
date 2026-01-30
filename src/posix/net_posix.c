#include <banjo/assert.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/net.h>

#include "check.h"

#include <arpa/inet.h>  // inet_ntop
#include <ifaddrs.h>   // getifaddrs, freeifaddrs
#include <net/if.h>    // IFF_UP, IFF_LOOPBACK
#include <netdb.h>     // addrinfo
#include <stdio.h>     // snprintf

struct bj_net_address {
    int type;
    union {
        struct in_addr  ipv4;
        struct in6_addr ipv6;
    };
};

void bj_begin_network(void) {
    // EMPTY
}

void bj_end_network(void) {
    // EMPTY
}

BANJO_EXPORT struct bj_net_address_info* bj_get_local_addresses(void) {
    struct ifaddrs* iflist;
    if (getifaddrs(&iflist) != 0) {
        bj_err("getifaddrs failed");
        return 0;
    }

    struct bj_net_address_info* head = 0;
    struct bj_net_address_info* tail = 0;

    for (struct ifaddrs* ifa = iflist; ifa; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)                    continue;
        if (!(ifa->ifa_flags & IFF_UP))        continue;
        if (ifa->ifa_flags & IFF_LOOPBACK)     continue;
        int family = ifa->ifa_addr->sa_family;
        if (family != AF_INET && family != AF_INET6) continue;

        struct bj_net_address* addr = bj_calloc(sizeof(*addr));
        if (!addr) break;

        if (family == AF_INET) {
            addr->type = AF_INET;
            addr->ipv4 = ((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
        } else {
            addr->type = AF_INET6;
            addr->ipv6 = ((struct sockaddr_in6*)ifa->ifa_addr)->sin6_addr;
        }

        struct bj_net_address_info* node = bj_calloc(sizeof(*node));
        if (!node) {
            bj_free(addr);
            break;
        }
        node->address = addr;
        node->next    = 0;

        if (tail) {
            tail->next = node;
        } else {
            head = node;
        }
        tail = node;
    }

    freeifaddrs(iflist);
    return head;
}

BANJO_EXPORT void bj_free_address_info(struct bj_net_address_info* info) {
    while (info) {
        struct bj_net_address_info* next = info->next;
        bj_free(info->address);
        bj_free(info);
        info = next;
    }
}

BANJO_EXPORT size_t bj_sprint_address(
    const struct bj_net_address* address,
    char*                        buffer,
    size_t                       buffer_size
) {
    bj_check_or_0(address);

    char tmp[INET6_ADDRSTRLEN];
    const char* result = 0;

    if (address->type == AF_INET) {
        result = inet_ntop(AF_INET, &address->ipv4, tmp, sizeof(tmp));
    } else if (address->type == AF_INET6) {
        result = inet_ntop(AF_INET6, &address->ipv6, tmp, sizeof(tmp));
    }

    if (!result) {
        return snprintf(buffer, buffer_size, "(unknown)");
    }

    return snprintf(buffer, buffer_size, "%s", tmp);
}


