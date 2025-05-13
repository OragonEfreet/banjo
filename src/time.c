#include <banjo/time.h>
#include <banjo/video.h>

extern bj_video_layer* s_video;

double bj_get_time(
    void
) {
    return (double)(bj_get_time_counter() - s_video->timer_base) / bj_get_time_frequency();
}
