////////////////////////////////////////////////////////////////////////////////
// audio_pcm.c
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/api.h>
#include <banjo/audio.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/time.h>

bj_audio_play_note_data data = {0};
bj_audio_device* p_device    = 0;

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if (!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    }

    data.function = BJ_AUDIO_PLAY_SINE;

    p_device = bj_open_audio_device(&(bj_audio_properties){
        .format      = BJ_AUDIO_FORMAT_F32,  /* float buffer */
        .amplitude   = 16000,                /* used only for INT16 path */
        .sample_rate = 44100,
        .channels    = 2,
    }, bj_audio_play_note, &data, &p_error);

    if (!p_device) {
        if (p_error) bj_err("cannot open audio: %s (%x)", p_error->message, p_error->code);
        return bj_callback_exit_error;
    }

    bj_audio_device_play(p_device);
    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;

    static const double melody[] = {
        261.63, 293.66, 329.63,
        349.23, 392.00, 349.23,
        329.63, 293.66, 261.63
    };
    enum { MELODY_LEN = 9 };

    int note = (int)bj_get_run_time();  /* changes once per second */

    if (note >= MELODY_LEN)
        return bj_callback_exit_success;

    data.frequency = melody[note];
    return bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    if (p_device) bj_close_audio_device(p_device);
    bj_end(0);
    return status;
}
