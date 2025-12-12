#include <banjo/assert.h>
#include <banjo/audio.h>
#include <banjo/system.h>
#include <banjo/video.h>




struct bj_video_layer* s_video = 0;
struct bj_audio_layer* s_audio = 0;

struct bj_video_layer* bj_begin_video(struct bj_error**);
void bj_end_video(struct bj_video_layer*, struct bj_error**);

struct bj_audio_layer* bj_begin_audio(struct bj_error**);
void bj_end_audio(struct bj_audio_layer*, struct bj_error**);

void bj_begin_time(void);
void bj_end_time(void);

void bj_begin_event(void);
void bj_end_event(void);

bj_bool bj_initialize(
    struct bj_error** p_error
) {
    struct bj_error* init_error = 0;

    bj_assert(s_video == 0);
    bj_assert(s_audio == 0);

    bj_begin_event();
    bj_begin_time();

    s_video = bj_begin_video(&init_error);
    if (init_error) {
        bj_err("cannot initialize: %s (code %x)", init_error->message, init_error->code);
        bj_forward_error(init_error, p_error);
        bj_shutdown(0);
        return BJ_FALSE;
    }

    s_audio = bj_begin_audio(&init_error);
    if (init_error) {
        bj_err("cannot initialize: %s (code %x)", init_error->message, init_error->code);
        bj_forward_error(init_error, p_error);
        bj_shutdown(0);
        return BJ_FALSE;
    }

    return BJ_TRUE;
}

void bj_shutdown(
    struct bj_error** p_error
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

