#define true 1
#define false 0

#ifndef SDL_H
#define SDL_H

#include <SDL2/SDL.h>
#include <stdint.h>

#define KEY_W          (1 << 0)
#define KEY_A          (1 << 1)
#define KEY_S          (1 << 2)
#define KEY_D          (1 << 3)
#define KEY_E          (1 << 4)
#define KEY_Q          (1 << 5)
#define KEY_SHIFT      (1 << 6)
#define KEY_CTRL       (1 << 7)
#define KEY_SPACE      (1 << 8)
#define KEY_RETURN     (1 << 9)
#define KEY_LEFT       (1 << 10)
#define KEY_RIGHT      (1 << 11)
#define KEY_UP         (1 << 12)
#define KEY_DOWN       (1 << 13)

typedef uint32_t InputMask;

// Initialisiert SDL, Fenster, Renderer und gibt den ersten Pixelbuffer zurück
uint32_t* outputInit(int width, int height);

// Zeigt den aktuellen Buffer an und gibt einen neuen leeren Backbuffer zurück
uint32_t* flipBuffer(void);

// Initialisiert Input-System (intern)
void inputInit(void);

// Gibt zurück, ob das Programm beendet werden soll (z. B. ESC oder Fenster schließen)
int shouldQuit(void);

// Verarbeitet Events und aktualisiert den Quit-Status (muss einmal pro Frame aufgerufen werden!)
InputMask updateInput(void);

int getMouseDeltaX(void);
int getMouseDeltaY(void);

void cleanup(void);

#endif // SDL_H
