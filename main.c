#include "emulator.h"
#include "ui.h"
#include "audio.h"
#include "video.h"
#include "file_utils.h"
#include "common.h"
#include <SDL2/SDL.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    if (init_ui() < 0) {
        fprintf(stderr, "Failed to initialize UI\n");
        return -1;
    }

    if (init_audio() < 0) {
        fprintf(stderr, "Failed to initialize audio\n");
        cleanup_ui();
        return -1;
    }

    if (init_video() < 0) {
        fprintf(stderr, "Failed to initialize video\n");
        cleanup_audio();
        cleanup_ui();
        return -1;
    }

    const char* firmware_path = "assets\\firmware\\";
    run_ui_loop(firmware_path);
    cleanup_emulator();
    cleanup_video();
    cleanup_audio();
    cleanup_ui();

    return 0;
}