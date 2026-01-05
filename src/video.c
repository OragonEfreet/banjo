#include <banjo/log.h>

#include "video_layer.h"

extern struct bj_video_layer_create_info emscripten_video_layer_info;
extern struct bj_video_layer_create_info win32_video_layer_info;
extern struct bj_video_layer_create_info x11_video_layer_info;
extern struct bj_video_layer_create_info cocoa_video_layer_info;

bj_bool bj_begin_video(
    struct bj_video_layer* vt,
    struct bj_error** p_error
) {
    static const struct bj_video_layer_create_info* layer_infos[] = {
#ifdef BJ_CONFIG_EMSCRIPTEN_BACKEND
        &emscripten_video_layer_info,
#endif
#ifdef BJ_CONFIG_WIN32_BACKEND
        &win32_video_layer_info,
#endif
#ifdef BJ_CONFIG_COCOA_BACKEND
        &cocoa_video_layer_info,
#endif
#ifdef BJ_CONFIG_X11_BACKEND
        &x11_video_layer_info,
#endif
    };

    const size_t n_layers = sizeof(layer_infos) / sizeof(struct bj_video_layer_create_info*);

    for(size_t b = 0 ; b < n_layers ; ++b) {

        struct bj_error* sub_err = 0;

        const struct bj_video_layer_create_info* p_create_info = layer_infos[b];
        const bj_bool success = p_create_info->create(vt, &sub_err);

        if(sub_err) {
            bj_err("while trying %s video layer: %s (code 0x%08X)",
                p_create_info->name, sub_err->message, sub_err->code
            );
            bj_clear_error(&sub_err);
        }

        if(success) {
            bj_info("video selected: %s", p_create_info->name);
            return BJ_TRUE;
        }
    }

    bj_set_error(p_error, BJ_ERROR_INITIALIZE, "no suitable video");
    return BJ_FALSE;
}

