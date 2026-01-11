////////////////////////////////////////////////////////////////////////////////
/// \example audio_pcm.c
/// Procedural audio synthesis and playback.
///
/// Banjo's audio system works with callbacks that fill PCM buffers. You open
/// an audio device with specific properties (sample rate, channels, format),
/// provide a callback function that generates audio samples, and Banjo calls
/// your function repeatedly to fill the audio buffer. This example uses
/// bj_play_audio_note to synthesize sine wave tones at different frequencies,
/// playing a simple melody.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/api.h>
#include <banjo/audio.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/time.h>

// Data passed to the audio callback. The callback reads this to know what
// frequency to generate. We modify it in real-time to change the pitch.
bj_audio_play_note_data data = {0};
bj_audio_device* p_device    = 0;

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    // Initialize the audio subsystem.
    if (!bj_begin(BJ_AUDIO_SYSTEM, 0)) {
        return bj_callback_exit_error;
    }

    // Choose which waveform to generate. BJ_AUDIO_PLAY_SINE creates smooth
    // sine wave tones. Other options include square, triangle, and sawtooth.
    data.function = BJ_AUDIO_PLAY_SINE;

    // Open an audio device with specific properties:
    //   - format: BJ_AUDIO_FORMAT_F32 uses 32-bit float samples (-1.0 to 1.0)
    //   - sample_rate: 44100 Hz (CD quality)
    //   - channels: 2 (stereo output)
    // The callback bj_play_audio_note will be called repeatedly to fill buffers.
    // We pass &data so the callback knows what frequency to generate.
    p_device = bj_open_audio_device(&(bj_audio_properties){
        .format      = BJ_AUDIO_FORMAT_F32,  /* float buffer */
        .amplitude   = 16000,                /* used only for INT16 path */
        .sample_rate = 44100,
        .channels    = 2,
    }, bj_play_audio_note, &data, 0);

    if (!p_device) {
        return bj_callback_exit_error;
    }

    // Start audio playback. The callback will now run in a separate thread,
    // continuously filling audio buffers.
    bj_play_audio_device(p_device);
    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;

    // Simple melody: C-D-E-F-G-F-E-D-C (frequencies in Hz).
    // Middle C (C4) = 261.63 Hz, D4 = 293.66 Hz, etc.
    static const double melody[] = {
        261.63, 293.66, 329.63,
        349.23, 392.00, 349.23,
        329.63, 293.66, 261.63
    };
    enum { MELODY_LEN = 9 };

    // Use runtime as note index - changes once per second.
    int note = (int)bj_run_time();

    if (note >= MELODY_LEN)
        return bj_callback_exit_success;

    // Update the frequency. The audio callback runs in a separate thread and
    // will immediately pick up this change, smoothly transitioning to the new note.
    data.frequency = melody[note];
    return bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    // Stop audio playback and close the device.
    if (p_device) bj_close_audio_device(p_device);
    bj_end();
    return status;
}
