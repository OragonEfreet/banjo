#include <banjo/assert.h>
#include <banjo/audio.h>
#include <banjo/system.h>

#include "audio_layer.h"
#include "video_layer.h"

struct bj_audio_layer s_audio = {0};
struct bj_video_layer s_video = {0};

bj_bool bj_begin_video(struct bj_video_layer* layer, struct bj_error**);
bj_bool bj_begin_audio(struct bj_audio_layer* layer, struct bj_error**);

void bj_begin_time(void);
void bj_end_time(void);

void bj_begin_event(void);
void bj_end_event(void);

bj_bool bj_initialize(
    struct bj_error** p_error
) {
    struct bj_error* init_error = 0;

    bj_begin_event();
    bj_begin_time();

    if (bj_begin_video(&s_video, &init_error) == BJ_FALSE) {
        if(init_error) {
            bj_err("cannot initialize: %s (code %x)", init_error->message, init_error->code);
            bj_forward_error(init_error, p_error);
        } else {
            bj_err("cannot initialize video layer");
        }

        bj_shutdown(0);
        return BJ_FALSE;
    }

    if (bj_begin_audio(&s_audio, &init_error) == BJ_FALSE) {
        if(init_error) {
            bj_err("cannot initialize: %s (code %x)", init_error->message, init_error->code);
            bj_forward_error(init_error, p_error);
        } else {
            bj_err("cannot initialize audio layer");
        }

        bj_shutdown(0);
        return BJ_FALSE;
    }

    return BJ_TRUE;
}

void bj_shutdown(
    struct bj_error** p_error
) {

    // Dispose audio
    s_audio.end(p_error);
    bj_info("closed audio backend");
    bj_memzero(&s_audio, sizeof(struct bj_audio_layer));

    s_video.end(p_error);
    bj_info("closed video backend");
    bj_memzero(&s_video, sizeof(struct bj_video_layer));

    bj_end_time();
    bj_end_event();
}

