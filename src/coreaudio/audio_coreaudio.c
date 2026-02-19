#ifdef BJ_CONFIG_COREAUDIO_BACKEND

#include <banjo/audio.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <audio.h>
#include <audio_layer.h>
#include <check.h>

#include <AudioToolbox/AudioToolbox.h>
#include <pthread.h>

// Note: This file is AI-generated.

////////////////////////////////////////////////////////////////////////////////
/// Device Structure
////////////////////////////////////////////////////////////////////////////////

struct coreaudio_device {
    struct bj_audio_device common;        // MUST be first member
    AudioQueueRef          audio_queue;
    AudioQueueBufferRef*   buffers;
    uint64_t               sample_index;
    unsigned               buffer_count;
    unsigned               frames_per_buffer;
    size_t                 bytes_per_sample;
    size_t                 buffer_size_bytes;
    pthread_mutex_t        lock;
    volatile bj_bool       initialized;
};

////////////////////////////////////////////////////////////////////////////////
/// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

static void coreaudio_output_callback(
    void*                  user_data,
    AudioQueueRef          queue,
    AudioQueueBufferRef    buffer
);

static void fill_with_silence(
    AudioQueueBufferRef         buffer,
    struct coreaudio_device*    ca_dev
);

////////////////////////////////////////////////////////////////////////////////
/// Format Mapping
////////////////////////////////////////////////////////////////////////////////

static AudioStreamBasicDescription create_asbd(
    const struct bj_audio_properties* props
) {
    AudioStreamBasicDescription asbd = {0};

    asbd.mSampleRate       = props->sample_rate;
    asbd.mChannelsPerFrame = props->channels;
    asbd.mFramesPerPacket  = 1;

    switch (props->format) {
        case BJ_AUDIO_FORMAT_INT16:
            asbd.mFormatID       = kAudioFormatLinearPCM;
            asbd.mFormatFlags    = kLinearPCMFormatFlagIsSignedInteger |
                                   kLinearPCMFormatFlagIsPacked;
            asbd.mBitsPerChannel = 16;
            asbd.mBytesPerFrame  = 2 * props->channels;
            asbd.mBytesPerPacket = asbd.mBytesPerFrame;
            break;

        case BJ_AUDIO_FORMAT_F32:
            asbd.mFormatID       = kAudioFormatLinearPCM;
            asbd.mFormatFlags    = kLinearPCMFormatFlagIsFloat |
                                   kLinearPCMFormatFlagIsPacked;
            asbd.mBitsPerChannel = 32;
            asbd.mBytesPerFrame  = 4 * props->channels;
            asbd.mBytesPerPacket = asbd.mBytesPerFrame;
            break;

        default:
            // Default to INT16 if unknown format
            asbd.mFormatID       = kAudioFormatLinearPCM;
            asbd.mFormatFlags    = kLinearPCMFormatFlagIsSignedInteger |
                                   kLinearPCMFormatFlagIsPacked;
            asbd.mBitsPerChannel = 16;
            asbd.mBytesPerFrame  = 2 * props->channels;
            asbd.mBytesPerPacket = asbd.mBytesPerFrame;
            break;
    }

    return asbd;
}

////////////////////////////////////////////////////////////////////////////////
/// Helper Functions
////////////////////////////////////////////////////////////////////////////////

static void fill_with_silence(
    AudioQueueBufferRef         buffer,
    struct coreaudio_device*    ca_dev
) {
    uint32_t silence = ca_dev->common.silence;

    if (silence == 0) {
        // Fast path: zero out entire buffer
        bj_memset(buffer->mAudioData, 0, ca_dev->buffer_size_bytes);
    } else {
        // Handle non-zero silence (e.g., unsigned 8-bit)
        size_t total_samples = ca_dev->frames_per_buffer *
                               ca_dev->common.properties.channels;
        uint8_t* dst = (uint8_t*)buffer->mAudioData;
        size_t bps = ca_dev->bytes_per_sample;

        for (size_t i = 0; i < total_samples; i++) {
            bj_memcpy(dst + i * bps, &silence, bps);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Audio Queue Callback
////////////////////////////////////////////////////////////////////////////////

static void coreaudio_output_callback(
    void*                  user_data,
    AudioQueueRef          queue,
    AudioQueueBufferRef    buffer
) {
    struct coreaudio_device* ca_dev = (struct coreaudio_device*)user_data;
    struct bj_audio_device* dev = &ca_dev->common;

    // Early exit if closing or not ready
    if (dev->should_close || !ca_dev->initialized) {
        return;
    }

    // Lock for thread safety
    pthread_mutex_lock(&ca_dev->lock);

    // Handle reset request
    if (dev->should_reset) {
        ca_dev->sample_index = 0;
        dev->should_reset = BJ_FALSE;
    }

    // Fill buffer based on playing state
    if (dev->playing) {
        // Generate audio via user callback
        dev->callback(
            buffer->mAudioData,
            ca_dev->frames_per_buffer,
            &dev->properties,
            dev->callback_user_data,
            ca_dev->sample_index
        );
        ca_dev->sample_index += ca_dev->frames_per_buffer;
    } else {
        // Output silence when paused
        fill_with_silence(buffer, ca_dev);
    }

    // Set buffer size
    buffer->mAudioDataByteSize = (UInt32)ca_dev->buffer_size_bytes;

    pthread_mutex_unlock(&ca_dev->lock);

    // Re-enqueue buffer (critical - don't forget!)
    AudioQueueEnqueueBuffer(queue, buffer, 0, NULL);
}

////////////////////////////////////////////////////////////////////////////////
/// Device Lifecycle - Open
////////////////////////////////////////////////////////////////////////////////

static struct bj_audio_device* coreaudio_open_device(
    const struct bj_audio_properties* p_properties,
    bj_audio_callback_fn              p_callback,
    void*                             p_callback_user_data,
    struct bj_error**                 p_error
) {
    // 1. Allocate device
    struct coreaudio_device* ca_dev = bj_calloc(sizeof(struct coreaudio_device));
    if (!ca_dev) {
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot allocate audio device");
        return NULL;
    }

    // 2. Initialize common fields
    ca_dev->common.callback           = p_callback;
    ca_dev->common.callback_user_data = p_callback_user_data;
    ca_dev->common.properties.format      = p_properties ? p_properties->format : BJ_AUDIO_FORMAT_INT16;
    ca_dev->common.properties.amplitude   = p_properties ? p_properties->amplitude : BJ_AUDIO_AMPLITUDE;
    ca_dev->common.properties.channels    = p_properties ? p_properties->channels : BJ_AUDIO_CHANNELS;
    ca_dev->common.properties.sample_rate = p_properties ? p_properties->sample_rate : BJ_AUDIO_SAMPLE_RATE;
    ca_dev->common.silence = 0; // Both INT16 and F32 use 0 for silence
    ca_dev->common.playing = BJ_TRUE; // Start playing immediately

    // 3. Set buffer parameters 
    ca_dev->buffer_count      = 3;
    ca_dev->frames_per_buffer = 512;
    ca_dev->bytes_per_sample  = BJ_AUDIO_FORMAT_WIDTH(ca_dev->common.properties.format) / 8;
    ca_dev->buffer_size_bytes = ca_dev->frames_per_buffer *
                                ca_dev->common.properties.channels *
                                ca_dev->bytes_per_sample;
    ca_dev->sample_index      = 0;

    // 4. Create Audio Stream Basic Description
    AudioStreamBasicDescription asbd = create_asbd(&ca_dev->common.properties);

    // 5. Create Audio Queue
    OSStatus status = AudioQueueNewOutput(
        &asbd,
        coreaudio_output_callback,
        ca_dev,
        NULL,                    // NULL = AudioQueue creates internal thread
        kCFRunLoopCommonModes,
        0,
        &ca_dev->audio_queue
    );

    if (status != noErr) {
        bj_set_error(p_error, BJ_ERROR_AUDIO, "AudioQueueNewOutput failed");
        bj_free(ca_dev);
        return NULL;
    }

    // 6. Allocate buffers array
    ca_dev->buffers = bj_calloc(sizeof(AudioQueueBufferRef) * ca_dev->buffer_count);
    if (!ca_dev->buffers) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot allocate buffer array");
        AudioQueueDispose(ca_dev->audio_queue, true);
        bj_free(ca_dev);
        return NULL;
    }

    // 7. Allocate and enqueue buffers
    for (unsigned i = 0; i < ca_dev->buffer_count; i++) {
        status = AudioQueueAllocateBuffer(
            ca_dev->audio_queue,
            (UInt32)ca_dev->buffer_size_bytes,
            &ca_dev->buffers[i]
        );

        if (status != noErr) {
            bj_set_error(p_error, BJ_ERROR_AUDIO, "AudioQueueAllocateBuffer failed");

            // Free already allocated buffers
            for (unsigned j = 0; j < i; j++) {
                AudioQueueFreeBuffer(ca_dev->audio_queue, ca_dev->buffers[j]);
            }
            AudioQueueDispose(ca_dev->audio_queue, true);
            bj_free(ca_dev->buffers);
            bj_free(ca_dev);
            return NULL;
        }

        // Fill with silence and enqueue
        fill_with_silence(ca_dev->buffers[i], ca_dev);
        ca_dev->buffers[i]->mAudioDataByteSize = (UInt32)ca_dev->buffer_size_bytes;

        status = AudioQueueEnqueueBuffer(ca_dev->audio_queue, ca_dev->buffers[i], 0, NULL);
        if (status != noErr) {
            bj_set_error(p_error, BJ_ERROR_AUDIO, "AudioQueueEnqueueBuffer failed");

            // Free all buffers
            for (unsigned j = 0; j <= i; j++) {
                AudioQueueFreeBuffer(ca_dev->audio_queue, ca_dev->buffers[j]);
            }
            AudioQueueDispose(ca_dev->audio_queue, true);
            bj_free(ca_dev->buffers);
            bj_free(ca_dev);
            return NULL;
        }
    }

    // 8. Initialize mutex
    if (pthread_mutex_init(&ca_dev->lock, NULL) != 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot initialize mutex");

        // Free all buffers
        for (unsigned i = 0; i < ca_dev->buffer_count; i++) {
            AudioQueueFreeBuffer(ca_dev->audio_queue, ca_dev->buffers[i]);
        }
        AudioQueueDispose(ca_dev->audio_queue, true);
        bj_free(ca_dev->buffers);
        bj_free(ca_dev);
        return NULL;
    }

    ca_dev->initialized = BJ_TRUE;

    // 9. Start the audio queue
    status = AudioQueueStart(ca_dev->audio_queue, NULL);
    if (status != noErr) {
        bj_set_error(p_error, BJ_ERROR_AUDIO, "AudioQueueStart failed");

        pthread_mutex_destroy(&ca_dev->lock);
        for (unsigned i = 0; i < ca_dev->buffer_count; i++) {
            AudioQueueFreeBuffer(ca_dev->audio_queue, ca_dev->buffers[i]);
        }
        AudioQueueDispose(ca_dev->audio_queue, true);
        bj_free(ca_dev->buffers);
        bj_free(ca_dev);
        return NULL;
    }

    bj_info("CoreAudio device opened: %u Hz, %u channels, format %d",
        ca_dev->common.properties.sample_rate,
        ca_dev->common.properties.channels,
        ca_dev->common.properties.format
    );

    return (struct bj_audio_device*)ca_dev;
}

////////////////////////////////////////////////////////////////////////////////
/// Device Lifecycle - Close
////////////////////////////////////////////////////////////////////////////////

static void coreaudio_close_device(struct bj_audio_device* dev) {
    if (!dev) {
        return;
    }

    struct coreaudio_device* ca_dev = (struct coreaudio_device*)dev;

    // 1. Signal shutdown
    dev->should_close = BJ_TRUE;

    // 2. Stop audio queue synchronously (waits for callbacks to finish)
    if (ca_dev->audio_queue) {
        AudioQueueStop(ca_dev->audio_queue, true); // Synchronous stop
    }

    // 3. Free buffers
    if (ca_dev->buffers) {
        for (unsigned i = 0; i < ca_dev->buffer_count; i++) {
            if (ca_dev->buffers[i]) {
                AudioQueueFreeBuffer(ca_dev->audio_queue, ca_dev->buffers[i]);
            }
        }
        bj_free(ca_dev->buffers);
    }

    // 4. Dispose audio queue
    if (ca_dev->audio_queue) {
        AudioQueueDispose(ca_dev->audio_queue, true);
    }

    // 5. Destroy mutex
    if (ca_dev->initialized) {
        pthread_mutex_destroy(&ca_dev->lock);
    }

    // 6. Free device
    bj_free(ca_dev);

    bj_info("CoreAudio device closed");
}

////////////////////////////////////////////////////////////////////////////////
/// Backend Lifecycle - Dispose
////////////////////////////////////////////////////////////////////////////////

static void coreaudio_dispose_audio(struct bj_error** p_error) {
    (void)p_error;
    // No cleanup needed - AudioToolbox is always available
    bj_info("CoreAudio backend disposed");
}

////////////////////////////////////////////////////////////////////////////////
/// Backend Initialization
////////////////////////////////////////////////////////////////////////////////

static bj_bool coreaudio_init_audio(
    struct bj_audio_layer* layer,
    struct bj_error** p_error
) {
    (void)p_error;

    layer->end          = coreaudio_dispose_audio;
    layer->open_device  = coreaudio_open_device;
    layer->close_device = coreaudio_close_device;

    return BJ_TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// Backend Registration
////////////////////////////////////////////////////////////////////////////////

struct bj_audio_layer_create_info coreaudio_audio_layer_info = {
    .name   = "coreaudio",
    .create = coreaudio_init_audio,
};

#endif // BJ_CONFIG_COREAUDIO_BACKEND
