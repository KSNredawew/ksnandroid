#include "audio.h"
#include <SDL2/SDL.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <stdio.h>

int init_audio(void) {
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL audio could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    avformat_network_init();
    return 0;
}

void cleanup_audio(void) {
    avformat_network_deinit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}