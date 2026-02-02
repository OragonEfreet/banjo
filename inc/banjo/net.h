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
struct bj_tcp_stream;

BANJO_EXPORT struct bj_tcp_listener* bj_listen_tcp(
    const struct bj_net_addr* addr,
    uint16_t                  port,
    struct bj_error**         error
);

BANJO_EXPORT struct bj_tcp_stream* bj_accept_tcp(
    struct bj_tcp_listener* listener,
    struct bj_error**         error
);

BANJO_EXPORT int bj_tcp_recv(
    struct bj_tcp_stream* stream,
    void*                 buf,
    size_t                len
);

BANJO_EXPORT int bj_tcp_send(
    struct bj_tcp_stream* stream,
    const void*           buf,
    size_t                len
);

BANJO_EXPORT void bj_close_tcp_stream(
    struct bj_tcp_stream* stream
);

BANJO_EXPORT void bj_close_tcp_listener(
    struct bj_tcp_listener* listener
);

#endif
/// \} // End of net group

