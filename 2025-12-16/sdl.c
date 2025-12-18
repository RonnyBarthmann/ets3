#include "sdl.h"
#include <stdio.h>
#include <stdlib.h>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* backTexture = NULL;
static uint32_t* currentBuffer = NULL;
static int pitch = 0;

static int quitRequested = 0;

uint32_t* outputInit(int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow("SDL2 2D Programm",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width, height,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        exit(1);
    }

    backTexture = SDL_CreateTexture(renderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    width, height);
    if (!backTexture) {
        printf("SDL_CreateTexture Error: %s\n", SDL_GetError());
        exit(1);
    }

    // Ersten Buffer locken
    SDL_LockTexture(backTexture, NULL, (void**)&currentBuffer, &pitch);

    quitRequested = 0;

    return currentBuffer;
}

uint32_t* flipBuffer(void) {
    SDL_UnlockTexture(backTexture);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backTexture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Neuen Buffer für nächsten Frame
    SDL_LockTexture(backTexture, NULL, (void**)&currentBuffer, &pitch);

    return currentBuffer;
}

void inputInit(void) {
    quitRequested = 0;
}

void updateInput(void) {
    SDL_Event event;

    // Alle Events verarbeiten
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quitRequested = 1;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                quitRequested = 1;
            }
        }
    }
}

int shouldQuit(void) {
    return quitRequested;
}

// Optional: Aufräumen
void cleanup(void) {
    if (backTexture) SDL_DestroyTexture(backTexture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
