#include <banjo/assert.h>
#include <banjo/audio.h>
#include <banjo/system.h>
#include <banjo/video.h>

#include "config.h"


bj_video_layer* s_video = 0;
bj_audio_layer* s_audio = 0;

bj_video_layer* bj_begin_video(bj_error**);
void bj_end_video(bj_video_layer*, bj_error**);

bj_audio_layer* bj_begin_audio(bj_error**);
void bj_end_audio(bj_audio_layer*, bj_error**);

void bj_begin_time(void);
void bj_end_time(void);

void bj_begin_event(void);
void bj_end_event(void);

bj_bool bj_initialize(
    bj_error** p_error
) {
    bj_error* error = 0;

    bj_assert(s_video == 0);
    bj_assert(s_audio == 0);

    bj_begin_event();
    bj_begin_time();

    s_video = bj_begin_video(&error);
    if (error) {
        bj_err("cannot initialize: %s (code %x)", error->message, error->code);
        bj_forward_error(error, p_error);
        bj_shutdown(0);
        return BJ_FALSE;
    }

    s_audio = bj_begin_audio(&error);
    if (error) {
        bj_err("cannot initialize: %s (code %x)", error->message, error->code);
        bj_forward_error(error, p_error);
        bj_shutdown(0);
        return BJ_FALSE;
    }
    
    return BJ_TRUE;
}

void bj_shutdown(
    bj_error** p_error
) {

    // Dispose audio
    if (s_audio) {
        bj_end_audio(s_audio, p_error);
        s_audio = 0;
    }

    if (s_video) {
        bj_end_video(s_video, p_error);
        s_video = 0;
    }

    bj_end_time();
    bj_end_event();
}

