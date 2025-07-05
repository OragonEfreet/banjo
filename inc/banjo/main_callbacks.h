#pragma once

typedef enum bj_callback_result_t
{
    bj_callback_exit_error   = -1,
    bj_callback_exit_success =  0,
    bj_callback_continue     =  1,
} bj_callback_result;

typedef bj_callback_result (*bj_app_begin_fn_t)(void**, int, char*[]);
typedef bj_callback_result (*bj_app_iterate_fn_t)(void*);
typedef bj_callback_result (*bj_app_end_fn_t)(void*, int);


