#include <banjo/api.h>
#include <banjo/audio.h>
#include <banjo/log.h>
#include <banjo/system.h>
#include <banjo/time.h>

#include <math.h>
#include <stdio.h>

#include <math.h>
#include <stdint.h>

typedef struct Note {
    double freq;
    double duration; // seconds
} Note;

#define D5  587.33
#define D6  1174.66
#define A5  880.00
#define GS5 830.61
#define G5  783.99
#define F5  698.46
#define AS4 466.16
#define B4  493.88
#define C5  523.25
#define HUSH 0.0

#define ONE(N) {N, 0.125}
#define TWO(N) {N, 0.250}

static const Note megalovania_intro[] = {
    ONE(D5), ONE(D5), TWO(D6),
    TWO(A5), ONE(HUSH), ONE(GS5),
    ONE(HUSH), ONE(G5), ONE(HUSH), ONE(F5),
    ONE(HUSH), ONE(D5), ONE(F5), ONE(G5),

    ONE(C5), ONE(C5), TWO(D6),
    TWO(A5), ONE(HUSH), ONE(GS5),
    ONE(HUSH), ONE(G5), ONE(HUSH), ONE(F5),
    ONE(HUSH), ONE(D5), ONE(F5), ONE(G5),

    ONE(B4), ONE(B4), TWO(D6), 
    TWO(A5), ONE(HUSH), ONE(GS5),
    ONE(HUSH), ONE(G5), ONE(HUSH), ONE(F5),
    ONE(HUSH), ONE(D5), ONE(F5), ONE(G5),

    ONE(AS4), ONE(AS4), TWO(D6), 
    TWO(A5), ONE(HUSH), ONE(GS5),
    ONE(HUSH), ONE(G5), ONE(HUSH), ONE(F5),
    ONE(HUSH), ONE(D5), ONE(F5), ONE(G5),
};

void my_audio_callback(int16_t* buffer, unsigned frames, const bj_audio_properties* audio, void* user_data) {
    (void)user_data;

    static double time = 0.0;
    static size_t note_index = 0;

    double sr = (double)audio->sample_rate;
    double amplitude = (double)audio->amplitude;

    for (unsigned i = 0; i < frames; ++i) {
        if (note_index >= sizeof(megalovania_intro)/sizeof(Note)) {
            note_index -= sizeof(megalovania_intro)/sizeof(Note);
            buffer[i] = 0; // done
            continue;
        }

        Note current = megalovania_intro[note_index];

        // Advance note if needed
        if (time > current.duration) {
            time = 0.0;
            note_index++;
            if (note_index >= sizeof(megalovania_intro)/sizeof(Note)) {
                buffer[i] = 0;
                continue;
            }
            current = megalovania_intro[note_index];
        }

        // Synthesize tone
        double t = time;
        double sample = 0;

        if (current.freq > 0.0) {
            sample += 1.0 * sin(2.0 * M_PI * current.freq * t);
            sample += 0.5 * sin(2.0 * M_PI * 2 * current.freq * t) * exp(-3.0 * t);
            sample += 0.3 * sin(2.0 * M_PI * 3 * current.freq * t) * exp(-4.0 * t);
            sample += 0.1 * sin(2.0 * M_PI * 4 * current.freq * t) * exp(-5.0 * t);

            double env = exp(-2.0 * t);
            sample *= env;
        }

        buffer[i] = (int16_t)(sample * amplitude);
        time += 1.0 / sr;
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

        bj_sleep(10000);

        bj_close_audio_device(p_device);
        bj_end(&p_error);
    } else {
        bj_err("while starting banjo: %s (%x)", p_error->message, p_error->code);
    }

    return 0;
}

