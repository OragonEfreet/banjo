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
#include <banjo/renderer.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

bj_window* window               = 0;
bj_renderer* renderer           = 0;
bj_bitmap* bmp_sprite_sheet     = 0;

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bmp_sprite_sheet = bj_create_bitmap_from_file(BANJO_ASSETS_DIR"/bmp/gabe-idle-run.bmp", 0);

    bj_error* p_error = 0;

    if(!bj_begin(BJ_VIDEO_SYSTEM, &p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    renderer = bj_create_renderer(BJ_RENDERER_TYPE_SOFTWARE);
    window = bj_bind_window("sprite sheet - Banjo", 0, 0, 
        bj_bitmap_width(bmp_sprite_sheet) * 10,
        bj_bitmap_height(bmp_sprite_sheet) * 10,
        0
    );

    bj_renderer_configure(renderer, window);
    bj_set_key_callback(bj_close_on_escape, 0);

    bj_blit_stretched(bmp_sprite_sheet, 0, bj_get_framebuffer(renderer), 0, BJ_BLIT_OP_COPY);
    bj_present(renderer, window);

    return bj_callback_continue;
}


int bj_app_iterate(void* user_data) {
    (void)user_data;

    bj_dispatch_events();

    return bj_should_close_window(window) 
         ? bj_callback_exit_success 
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_destroy_renderer(renderer);
    bj_unbind_window(window);
    bj_end();

    bj_destroy_bitmap(bmp_sprite_sheet);
    return status;
}


