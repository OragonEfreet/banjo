////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for various event types
#pragma once

#include <banjo/api.h>

struct bj_window_t;

typedef void(* bj_key_callback_t)(struct bj_window_t*);

BANJO_EXPORT bj_key_callback_t bj_set_key_callback(
    struct bj_window_t* p_window,
    bj_key_callback_t   p_callback
);
