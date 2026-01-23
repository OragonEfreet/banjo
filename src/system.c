#include <banjo/assert.h>
#include <banjo/audio.h>
#include <banjo/system.h>

#include "audio_layer.h"
#include "video_layer.h"

struct bj_audio_layer s_audio = {0};
struct bj_video_layer s_video = {0};

static struct {
    unsigned short time;
    unsigned short event;
    unsigned short video;
    unsigned short audio;
    unsigned short network;
} syscount = {0};

static void retain_time(void) {
    if(syscount.time++ == 0) {
        bj_begin_time();
    }
}

static void release_time(void) {
    if(syscount.time > 0 && syscount.time-- == 1) {
        bj_end_time();
    }
}

static void retain_event(void) {
    if(syscount.event++ == 0) {
        bj_begin_event();
    }
}

static void release_event(void) {
    if(syscount.event > 0 && syscount.event-- == 1) {
        bj_end_event();
    }
}

static bj_bool retain_video(struct bj_error** p_error) {
    if(syscount.video == 0) {
        retain_event();
        if(bj_begin_video(&s_video, p_error) == BJ_FALSE) {
            release_event();
            return BJ_FALSE;
        }
    } else {
        retain_event();
    }
    syscount.video++;
    return BJ_TRUE;
}

static void release_video(struct bj_error** p_error) {
    if(syscount.video > 0) {
        if(syscount.video-- == 1) {
            bj_assert(s_video.end);
            s_video.end(p_error);
            bj_memzero(&s_video, sizeof(struct bj_video_layer));
        }
        release_event();
    }
}

static bj_bool retain_audio(struct bj_error** p_error) {
    if(syscount.audio == 0) {
        retain_event();
        if(bj_begin_audio(&s_audio, p_error) == BJ_FALSE) {
            release_event();
            return BJ_FALSE;
        }
    } else {
        retain_event();
    }
    syscount.audio++;
    return BJ_TRUE;
}

static void release_audio(struct bj_error** p_error) {
    if(syscount.audio > 0) {
        if(syscount.audio-- == 1) {
            bj_assert(s_audio.end);
            s_audio.end(p_error);
            bj_memzero(&s_audio, sizeof(struct bj_audio_layer));
        }
        release_event();
    }
}

static void retain_network(void) {
    if(syscount.network++ == 0) {
        bj_begin_network();
    }
}

static void release_network(void) {
    if(syscount.network > 0 && syscount.network-- == 1) {
        bj_end_network();
    }
}

bj_bool bj_begin(
    int               systems,
    struct bj_error** p_error
) {
    retain_time();
    retain_event();

    if((systems & BJ_VIDEO_SYSTEM) > 0) {
        if (retain_video(p_error) == BJ_FALSE) {
            release_event();
            release_time();
            return BJ_FALSE;
        }
    }

    if((systems & BJ_AUDIO_SYSTEM) > 0) {
        if (retain_audio(p_error) == BJ_FALSE) {
            if((systems & BJ_VIDEO_SYSTEM) > 0) {
                release_video(0);
            }
            release_event();
            release_time();
            return BJ_FALSE;
        }
    }

    if((systems & BJ_NETWORK_SYSTEM) > 0) {
        retain_network();
    }

    bj_trace("system initialized (time: %u, event: %u, audio: %u, video: %u, network: %u)",
        syscount.time,
        syscount.event,
        syscount.audio,
        syscount.video,
        syscount.network
    );

    return BJ_TRUE;
}

void bj_end(void) {
    if(syscount.audio > 0) {
        bj_assert(s_audio.end);
        s_audio.end(0);
        bj_memzero(&s_audio, sizeof(struct bj_audio_layer));
    }

    if(syscount.video > 0) {
        bj_assert(s_video.end);
        s_video.end(0);
        bj_memzero(&s_video, sizeof(struct bj_video_layer));
    }

    if(syscount.event > 0) {
        bj_end_event();
    }

    if(syscount.time > 0) {
        bj_end_time();
    }

    if(syscount.network > 0) {
        bj_end_network();
    }

    bj_trace("system shutdown (time: %u -> 0, event: %u -> 0, audio: %u -> 0, video: %u -> 0, network: %u -> 0)",
        syscount.time,
        syscount.event,
        syscount.audio,
        syscount.video,
        syscount.network
    );

    syscount.audio   = 0;
    syscount.video   = 0;
    syscount.event   = 0;
    syscount.time    = 0;
    syscount.network = 0;
}

bj_bool bj_begin_system(
    enum bj_system    system,
    struct bj_error** p_error
) {
    retain_time();
    retain_event();

    switch(system) {
        case BJ_VIDEO_SYSTEM:
            if (retain_video(p_error) == BJ_FALSE) {
                release_event();
                release_time();
                return BJ_FALSE;
            }
            break;
        case BJ_AUDIO_SYSTEM:
            if (retain_audio(p_error) == BJ_FALSE) {
                release_event();
                release_time();
                return BJ_FALSE;
            }
            break;
        case BJ_NETWORK_SYSTEM:
            retain_network();
            break;
        default:
            break;
    }

    bj_trace("system retained (time: %u, event: %u, audio: %u, video: %u, network: %u)",
        syscount.time,
        syscount.event,
        syscount.audio,
        syscount.video,
        syscount.network
    );

    return BJ_TRUE;
}

void bj_end_system(
    enum bj_system    system,
    struct bj_error** p_error
) {
    switch(system) {
        case BJ_VIDEO_SYSTEM:
            release_video(p_error);
            break;
        case BJ_AUDIO_SYSTEM:
            release_audio(p_error);
            break;
        case BJ_NETWORK_SYSTEM:
            release_network();
            break;
        default:
            break;
    }

    release_event();
    release_time();

    bj_trace("system released (time: %u, event: %u, audio: %u, video: %u, network: %u)",
        syscount.time,
        syscount.event,
        syscount.audio,
        syscount.video,
        syscount.network
    );
}

