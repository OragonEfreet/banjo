#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/video.h>

#include "config.h"

extern bj_video_layer_create_info fake_layer_info;
#if BJ_HAS_FEATURE(X11)
extern bj_video_layer_create_info x11_layer_info;
#endif
#if BJ_HAS_FEATURE(WIN32)
extern bj_video_layer_create_info win32_layer_info;
#endif

static const bj_video_layer_create_info* layer_infos[] = {
#if BJ_HAS_FEATURE(WIN32)
    &win32_layer_info,
#endif
#if BJ_HAS_FEATURE(X11)
    &x11_layer_info,
#endif
    &fake_layer_info,
};

bj_video_layer* bj_init_video(
    bj_error** p_error
) {
    const size_t n_layers = sizeof(layer_infos) / sizeof(bj_video_layer_create_info*);

    for(size_t b = 0 ; b < n_layers ; ++b) {

        bj_error* sub_err = 0;

        const bj_video_layer_create_info* p_create_info = layer_infos[b];
        bj_trace("Will try to initialize %s layer", p_create_info->name);
        bj_video_layer* p_layer = p_create_info->create(&sub_err);

        if(sub_err) {
            bj_message(p_layer == 0 ? 0 : 1, 0, 0,
                "Error while initializing %s: %s (code 0x%08X)",
                p_create_info->name, sub_err->message, sub_err->code
            );
            bj_clear_error(&sub_err);
        }

        if(p_layer != 0) {
            bj_info("Initialized %s system", p_create_info->name);
            return p_layer;
        }
    }

    bj_set_error(p_error, BJ_ERROR_INITIALIZE, "No suitable layer found");
    return 0;
}

void bj_dispose_video(
    bj_video_layer* p_video,
    bj_error** p_error
) {
    void (*dispose)(struct bj_video_layer_t*, bj_error** p_error) = p_video->dispose;
    dispose(p_video, p_error);
    bj_info("Disposed system");
}

