#include <banjo/log.h>
#include <banjo/video.h>

#include "config.h"

extern bj_video_layer_create_info emscripten_video_layer_info;
extern bj_video_layer_create_info novideo_video_layer_info;
extern bj_video_layer_create_info win32_video_layer_info;
extern bj_video_layer_create_info x11_video_layer_info;
extern bj_video_layer_create_info cocoa_video_layer_info;

static const bj_video_layer_create_info* layer_infos[] = {
#if BJ_HAS_FEATURE(EMSCRIPTEN)
    &emscripten_video_layer_info,
#endif
#if BJ_HAS_FEATURE(WIN32)
    &win32_video_layer_info,
#endif
#if BJ_HAS_FEATURE(COCOA)
    &cocoa_video_layer_info,
#endif
#if BJ_HAS_FEATURE(X11)
    &x11_video_layer_info,
#endif
    &novideo_video_layer_info,
};

bj_video_layer* bj_begin_video(
    bj_error** p_error
) {
    const size_t n_layers = sizeof(layer_infos) / sizeof(bj_video_layer_create_info*);

    for(size_t b = 0 ; b < n_layers ; ++b) {

        bj_error* sub_err = 0;

        const bj_video_layer_create_info* p_create_info = layer_infos[b];
        bj_video_layer* p_layer = p_create_info->create(&sub_err);

        if(sub_err) {
            bj_log_message(p_layer == 0 ? 0 : 1, 0, 0,
                "while trying %s video layer: %s (code 0x%08X)",
                p_create_info->name, sub_err->message, sub_err->code
            );
            bj_clear_error(&sub_err);
        }

        if(p_layer != 0) {
            bj_info("video: %s", p_create_info->name);
            return p_layer;
        }
    }

    bj_set_error(p_error, BJ_ERROR_INITIALIZE, "no suitable video");
    return 0;
}

void bj_end_video(
    bj_video_layer* p_video,
    bj_error** p_error
) {
    void (*end)(struct bj_video_layer_t*, bj_error** p_error) = p_video->end;
    end(p_video, p_error);
}

