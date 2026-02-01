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

struct bj_net_addr;
struct bj_tcp_listener;

BANJO_EXPORT void bj_unbind(
    struct bj_tcp_listener* listener
);

BANJO_EXPORT struct bj_tcp_listener* bj_bind(
    const struct bj_net_addr* addr,
    uint16_t                  port,
    uint16_t                  queue_size
);

#endif
/// \} // End of net group

