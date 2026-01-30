////////////////////////////////////////////////////////////////////////////////
/// \file net.h
/// Header file for base network API.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup net Network
/// \brief Basic network API
///
/// The net API provides a thin wrapping mechanism around POSIX sockets and
/// Winsocks for TCP and UDP network communication.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_NET_H
#define BJ_NET_H

#include <banjo/api.h>
#include <banjo/error.h>

enum bj_net_type {
    BJ_IPV4,
    BJ_IPV6,
};

struct bj_net_address;

struct bj_net_address_info {
    struct bj_net_address*      address;
    struct bj_net_address_info* next;
};

BANJO_EXPORT struct bj_net_address_info* bj_get_local_addresses(void);

BANJO_EXPORT void bj_free_address_info(
    struct bj_net_address_info* info
);

BANJO_EXPORT size_t bj_sprint_address(
    const struct bj_net_address* address,
    char*                        buffer,
    size_t                       buffer_size
);


#endif
/// \} // End of net group

