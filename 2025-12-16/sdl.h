#define true 1
#define false 0

#ifndef SDL_H
#define SDL_H

#include <SDL2/SDL.h>
#include <stdint.h>

// Initialisiert SDL, Fenster, Renderer und gibt den ersten Pixelbuffer zurück
uint32_t* outputInit(int width, int height);

// Zeigt den aktuellen Buffer an und gibt einen neuen leeren Backbuffer zurück
uint32_t* flipBuffer(void);

// Initialisiert Input-System (intern)
void inputInit(void);

// Gibt zurück, ob das Programm beendet werden soll (z. B. ESC oder Fenster schließen)
int shouldQuit(void);

// Verarbeitet Events und aktualisiert den Quit-Status (muss einmal pro Frame aufgerufen werden!)
void updateInput(void);

void cleanup(void);

#endif // SDL_H
