#include <banjo/api.h>
#include <banjo/audio.h>
#include <banjo/log.h>
#include <banjo/system.h>
#include <banjo/time.h>

#include <math.h>
#include <stdio.h>

void my_audio_callback(int16_t* buffer, unsigned frames, const bj_audio_properties* audio, void* user_data) {
    (void)user_data;
    static double phase = 0;
    double freq = 440.0;
    double sr = audio->sample_rate;
    for (unsigned int i = 0 ; i < frames; ++i) {
        buffer[i] = (int16_t)(audio->amplitude * sin(phase));
        phase += 2.0 * M_PI * freq / sr;
        if(phase > 2.0 * M_PI) {
            phase -= 2.0 * M_PI;
        }
    }
}

int main(void) {

    bj_error* p_error = 0;

    if (bj_begin(&p_error)) {

        bj_audio_device* p_device = bj_open_audio_device(&p_error, my_audio_callback, 0);
        
        if (p_device == 0) {
            if (p_error) {
                bj_err("cannot open audio: %s (%x)", p_error->message, p_error->code);
            }
            return 0;
        }

        bj_sleep(3000);

        bj_close_audio_device(p_device);
        bj_end(&p_error);
    } else {
        bj_err("while starting banjo: %s (%x)", p_error->message, p_error->code);
    }

    return 0;
}

