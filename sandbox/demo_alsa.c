
// Compile: gcc -o alsa_notes alsa_notes.c -lasound -lpthread

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define SAMPLE_RATE 44100
#define AMPLITUDE 16000
#define CHANNELS 1
#define FORMAT SND_PCM_FORMAT_S16_LE

#define PERIOD_FRAMES 512
#define BUFFER_FRAMES (PERIOD_FRAMES * 4)

volatile int running = 1;
snd_pcm_t *pcm_handle;

// Notes in Hz: A4, C5, E5, G5, A5
double notes[] = { 440.0, 523.25, 659.25, 783.99, 880.0 };
const int note_count = sizeof(notes) / sizeof(notes[0]);

void *audio_thread(void *arg) {
    (void)arg;
    int16_t buffer[PERIOD_FRAMES];
    int sample_index = 0;
    double current_freq = notes[0];
    int last_second = -1;

    while (running) {
        snd_pcm_sframes_t avail = snd_pcm_avail_update(pcm_handle);
        if (avail < 0) {
            if (avail == -EPIPE) {
                fprintf(stderr, "Underrun!\n");
                snd_pcm_prepare(pcm_handle);
                continue;
            } else {
                fprintf(stderr, "avail error: %s\n", snd_strerror(avail));
                break;
            }
        }

        // Change note every second
        int now = time(NULL);
        if (now != last_second) {
            last_second = now;
            current_freq = notes[now % note_count];
            printf("Switching to frequency: %.2f Hz\n", current_freq);
        }

        if (avail >= PERIOD_FRAMES) {
            for (int i = 0; i < PERIOD_FRAMES; ++i) {
                double t = (double)(sample_index++) / SAMPLE_RATE;
                buffer[i] = (int16_t)(AMPLITUDE * sin(2 * M_PI * current_freq * t));
            }

            int err = snd_pcm_writei(pcm_handle, buffer, PERIOD_FRAMES);
            if (err == -EPIPE) {
                fprintf(stderr, "Write underrun!\n");
                snd_pcm_prepare(pcm_handle);
            } else if (err < 0) {
                fprintf(stderr, "Write error: %s\n", snd_strerror(err));
                break;
            }
        } else {
            usleep(1000); // Yield a bit if not enough space
        }
    }

    return NULL;
}

int main() {
    snd_pcm_hw_params_t *params;
    unsigned int rate = SAMPLE_RATE;
    snd_pcm_uframes_t period = PERIOD_FRAMES;
    snd_pcm_uframes_t buffer_size = BUFFER_FRAMES;
    pthread_t thread;

    if (snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        perror("snd_pcm_open");
        return 1;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, FORMAT);
    snd_pcm_hw_params_set_channels(pcm_handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, NULL);
    snd_pcm_hw_params_set_period_size_near(pcm_handle, params, &period, NULL);
    snd_pcm_hw_params_set_buffer_size_near(pcm_handle, params, &buffer_size);

    if (snd_pcm_hw_params(pcm_handle, params) < 0) {
        perror("snd_pcm_hw_params");
        return 1;
    }

    pthread_create(&thread, NULL, audio_thread, NULL);

    sleep(6); // Let it run for a few note changes
    running = 0;

    pthread_join(thread, NULL);
    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);

    return 0;
}

