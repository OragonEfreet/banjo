#pragma once

#include <banjo/bitmap.h>

#include <SDL3/SDL.h>


int bj_pixel_mode_to_sdl(int mode);
int sdl_display_bitmap(bj_bitmap*, const char*, bool);
void sdl_what_is_the_format(const char*);
