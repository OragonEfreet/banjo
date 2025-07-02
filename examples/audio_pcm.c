#include <banjo/api.h>
#include <banjo/audio.h>
#include <banjo/log.h>
#include <banjo/system.h>
#include <banjo/time.h>


typedef struct {
    double frequency;
    int duration_ms;
} note_t;

int main(void) {
    bj_error* p_error = 0;

    if (bj_begin(&p_error)) {
        // Melody: C D E F G F E D C
        const note_t melody[] = {
            {261.63, 300}, {293.66, 300}, {329.63, 300},
            {349.23, 300}, {392.00, 300}, {349.23, 300},
            {329.63, 300}, {293.66, 300}, {261.63, 600}
        };

        bj_audio_play_note_data data = {0};

        bj_audio_device* p_device = bj_open_audio_device(&p_error, bj_audio_play_note, &data);
        if (p_device == 0) {
            if (p_error) {
                bj_err("cannot open audio: %s (%x)", p_error->message, p_error->code);
            }
            return 0;
        }

        for (size_t i = 0; i < sizeof(melody) / sizeof(melody[0]); ++i) {
            data.frequency = melody[i].frequency;
            bj_audio_device_play(p_device);
            bj_sleep(melody[i].duration_ms);
            bj_audio_device_stop(p_device);         // stop between notes
            bj_sleep(50);  // small pause between notes
        }

        bj_close_audio_device(p_device);
        bj_end(&p_error);
    }
    else {
        bj_err("while starting banjo: %s (%x)", p_error->message, p_error->code);
    }

    return 0;
}
