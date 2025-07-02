#include <banjo/assert.h>
#include <banjo/audio.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/video.h>

#include "config.h"


bj_video_layer* s_video = 0;
bj_audio_layer* s_audio = 0;

bj_video_layer* bj_init_video(bj_error**);
void bj_dispose_video(bj_video_layer*, bj_error**);
bj_audio_layer* bj_init_audio(bj_error**);
void bj_dispose_audio(bj_audio_layer*, bj_error**);
void bj_init_time(void);

bj_bool bj_begin(
    bj_error** p_error
) {
    bj_error* error = 0;

    bj_assert(s_video == 0);
    bj_assert(s_audio == 0);

    bj_init_time();

    s_video = bj_init_video(&error);
    if (error) {
        bj_err("cannot initialize: %s (code %x)", error->message, error->code);
        bj_forward_error(error, p_error);
        bj_end(0);
        return BJ_FALSE;
    }

    s_audio = bj_init_audio(&error);
    if (error) {
        bj_err("cannot initialize: %s (code %x)", error->message, error->code);
        bj_forward_error(error, p_error);
        bj_end(0);
        return BJ_FALSE;
    }
    
    return BJ_TRUE;
}

void bj_end(
    bj_error** p_error
) {

    // Dispose audio
    if (s_audio) {
        bj_dispose_audio(s_audio, p_error);
        s_audio = 0;
    }

    if (s_video) {
        bj_dispose_video(s_video, p_error);
        s_video = 0;
    }
}

