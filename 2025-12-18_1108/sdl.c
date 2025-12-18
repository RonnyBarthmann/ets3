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
    SDL_SetRelativeMouseMode(SDL_TRUE);      // Maus wird relativ → kein Cursor sichtbar
    SDL_ShowCursor(SDL_DISABLE);
}

// Bitmaske für Tasten (32-Bit reicht locker)
typedef uint32_t InputMask;

// Globale Variablen für Maus-Delta
static int mouseDeltaX = 0;
static int mouseDeltaY = 0;

// Haupt-Update-Funktion
InputMask updateInput(void) {
    SDL_Event event;
    InputMask input = 0;

    // Maus-Delta zurücksetzen
    mouseDeltaX = 0;
    mouseDeltaY = 0;

    // Alle Events verarbeiten
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quitRequested = 1;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    quitRequested = 1;
                }
                break;

            case SDL_KEYUP:
                // Optional: Tasten wieder freigeben
                // switch (event.key.keysym.scancode) { ... input &= ~KEY_XXX; }
                break;

            case SDL_MOUSEMOTION:
                mouseDeltaX = event.motion.xrel;
                mouseDeltaY = event.motion.yrel;
                break;
        }
    }
    
    // Tasten-Zustand direkt abfragen (das ist der entscheidende Teil!)
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

    if (keyboardState[SDL_SCANCODE_W])      input |= KEY_W;
    if (keyboardState[SDL_SCANCODE_A])      input |= KEY_A;
    if (keyboardState[SDL_SCANCODE_S])      input |= KEY_S;
    if (keyboardState[SDL_SCANCODE_D])      input |= KEY_D;
    if (keyboardState[SDL_SCANCODE_E])      input |= KEY_E;
    if (keyboardState[SDL_SCANCODE_Q])      input |= KEY_Q;
    if (keyboardState[SDL_SCANCODE_LSHIFT]) input |= KEY_SHIFT;
    if (keyboardState[SDL_SCANCODE_LCTRL])  input |= KEY_CTRL;
    if (keyboardState[SDL_SCANCODE_SPACE])  input |= KEY_SPACE;
    if (keyboardState[SDL_SCANCODE_RETURN]) input |= KEY_RETURN;
    if (keyboardState[SDL_SCANCODE_LEFT])   input |= KEY_LEFT;
    if (keyboardState[SDL_SCANCODE_RIGHT])  input |= KEY_RIGHT;
    if (keyboardState[SDL_SCANCODE_UP])     input |= KEY_UP;
    if (keyboardState[SDL_SCANCODE_DOWN])   input |= KEY_DOWN;
    
    return input;
}

// Zwei Hilfsfunktionen für Maus-Delta
int getMouseDeltaX(void) {
    return mouseDeltaX;
}

int getMouseDeltaY(void) {
    return mouseDeltaY;
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
