#include <banjo/api.h>
#include <banjo/audio.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>

#include <stdio.h>
#include <unistd.h>

// Simple sine wave test
struct test_data {
    int function;
    double frequency;
    double phase;
};

void my_app_begin(void** p_user_data, struct bj_error** p_error) {
    (void)p_user_data;

    bj_info("Testing CoreAudio backend on macOS");
    bj_info("Starting audio system...");

    if (!bj_begin(BJ_AUDIO_SYSTEM, p_error)) {
        bj_err("Failed to initialize audio system");
        return;
    }

    bj_info("Audio system initialized successfully!");
}

void my_app_iterate(void* user_data, struct bj_error** p_error) {
    (void)user_data;
    (void)p_error;
}

void my_app_end(void* user_data, struct bj_error** p_error) {
    (void)user_data;
    (void)p_error;
    bj_info("Shutting down...");
}

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    struct bj_error* error = NULL;

    // Initialize Banjo
    if (!bj_begin(BJ_AUDIO_SYSTEM, &error)) {
        if (error) {
            printf("Error initializing Banjo: %s\n", error->message);
            bj_clear_error(&error);
        }
        return 1;
    }

    printf("Audio system started!\n");

    // Set up audio properties
    struct bj_audio_properties props = {
        .format = BJ_AUDIO_FORMAT_INT16,
        .sample_rate = 44100,
        .channels = 1,
        .amplitude = 16000
    };

    // Set up note data for 440Hz (A4) sine wave
    struct bj_audio_play_note_data note_data = {
        .function = BJ_AUDIO_PLAY_SINE,
        .frequency = 440.0,
        .phase = 0.0
    };

    // Open audio device
    printf("Opening audio device...\n");
    struct bj_audio_device* device = bj_open_audio_device(
        &props,
        bj_play_audio_note,
        &note_data,
        &error
    );

    if (!device) {
        if (error) {
            printf("Error opening audio device: %s\n", error->message);
            bj_clear_error(&error);
        }
        bj_end();
        return 1;
    }

    printf("Audio device opened successfully!\n");
    printf("Playing 440 Hz sine wave for 3 seconds...\n");

    // Play for 3 seconds
    bj_play_audio_device(device);
    sleep(3);

    printf("Pausing for 1 second...\n");
    bj_pause_audio_device(device);
    sleep(1);

    printf("Resuming for 2 seconds...\n");
    bj_play_audio_device(device);
    sleep(2);

    printf("Stopping...\n");
    bj_stop_audio_device(device);

    printf("Closing audio device...\n");
    bj_close_audio_device(device);

    printf("Shutting down audio system...\n");
    bj_end();

    printf("Test completed successfully!\n");
    return 0;
}
