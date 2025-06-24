#include <banjo/api.h>
#include <banjo/audio.h>
#include <banjo/log.h>
#include <banjo/system.h>
#include <banjo/time.h>

#include <math.h>

int main(void) {

    bj_error* p_error = 0;

    if (bj_begin(&p_error)) {

        bj_audio_play_note_data data = { .frequency = 440.0, .function = BJ_AUDIO_PLAY_SINE };
        bj_audio_device* p_device = bj_open_audio_device(&p_error, bj_audio_play_note, &data);
        
        if (p_device == 0) {
            if (p_error) {
                bj_err("cannot open audio: %s (%x)", p_error->message, p_error->code);
            }
            return 0;
        }

        bj_sleep(200);
        bj_audio_device_play(p_device);
        bj_sleep(200);

        bj_close_audio_device(p_device);
        bj_end(&p_error);
    } else {
        bj_err("while starting banjo: %s (%x)", p_error->message, p_error->code);
    }

    return 0;
}

