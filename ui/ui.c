#include "ui.h"
#include "file_utils.h"
#include "emulator.h"
#include <SDL2/SDL.h>
#include <tinyfiledialogs.h>
#include <stdio.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 50
#define BUTTON_SPACING 20

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

int init_ui(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("KSNAndroid Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                             WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

void run_ui_loop(const char* firmware_path) {
    SDL_Event e;
    int quit = 0;
    SDL_Rect old_device_button = {WINDOW_WIDTH/2 - BUTTON_WIDTH - BUTTON_SPACING, WINDOW_HEIGHT/2 - BUTTON_HEIGHT/2, BUTTON_WIDTH, BUTTON_HEIGHT};
    SDL_Rect new_device_button = {WINDOW_WIDTH/2 + BUTTON_SPACING, WINDOW_HEIGHT/2 - BUTTON_HEIGHT/2, BUTTON_WIDTH, BUTTON_HEIGHT};

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                int is_new_device = 0;

                if (x >= old_device_button.x && x <= old_device_button.x + old_device_button.w &&
                    y >= old_device_button.y && y <= old_device_button.y + old_device_button.h) {
                    const char* selected_path = tinyfd_selectFolderDialog("Select Old Device Firmware Folder", "");
                    if (selected_path) {
                        copy_firmware_files(selected_path, firmware_path);
                        start_emulator(firmware_path, 0);
                    }
                } else if (x >= new_device_button.x && x <= new_device_button.x + new_device_button.w &&
                           y >= new_device_button.y && y <= new_device_button.y + new_device_button.h) {
                    is_new_device = 1;
                    const char* selected_path = tinyfd_selectFolderDialog("Select New Device Firmware Folder", "");
                    if (selected_path) {
                        copy_firmware_files(selected_path, firmware_path);
                        start_emulator(firmware_path, 1);
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Отрисовка кнопок
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &old_device_button);
        SDL_SetRenderDrawColor(renderer, 200, 200, 255, 255);
        SDL_RenderFillRect(renderer, &new_device_button);

        SDL_RenderPresent(renderer);
    }
}

void cleanup_ui(void) {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}