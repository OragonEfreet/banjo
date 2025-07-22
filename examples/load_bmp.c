////////////////////////////////////////////////////////////////////////////////
/// \example load_bmp.c
/// How to load and display a bitmap in a window
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/memory.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

bj_window* window               = 0;
bj_bitmap* p_window_framebuffer = 0;
bj_bitmap* bmp_sprite_sheet     = 0;

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bmp_sprite_sheet = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/gabe-idle-run.bmp", 0);

    bj_error* p_error = 0;

    if(!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    window = bj_window_new("sprite sheet - Banjo", 0, 0, 
        bj_bitmap_width(bmp_sprite_sheet) * 10,
        bj_bitmap_height(bmp_sprite_sheet) * 10,
        0
    );

    bj_set_key_callback(bj_close_on_escape);

    p_window_framebuffer = bj_window_get_framebuffer(window, 0);
    bj_bitmap_blit_stretched(bmp_sprite_sheet, 0, p_window_framebuffer, 0);
    bj_window_update_framebuffer(window);

    return bj_callback_continue;
}


int bj_app_iterate(void* user_data) {
    (void)user_data;

    bj_dispatch_events();

    return bj_window_should_close(window) 
         ? bj_callback_exit_success 
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_window_del(window);
    bj_end(0);

    bj_bitmap_del(bmp_sprite_sheet);
    return status;
}


