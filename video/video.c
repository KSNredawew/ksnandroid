#include "video.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <stdio.h>

int init_video(void) {
    av_register_all();
    return 0;
}

void cleanup_video(void) {
    // Cleanup FFmpeg resources
}