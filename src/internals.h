#pragma once

void* bj_load_library(
    const char* p_path
);

void bj_unload_library(
    void* p_handle
);

void* bj_get_symbol(
    void*       p_handle,
    const char* p_name
);


