#define true 1
#define false 0

#include "sdl.h"
#include "render.h"
#include "objects.h"
#include "models.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern struct triangle triangles[MAX_TRIANGLES];
extern struct triangle triangles_transformed[MAX_TRIANGLES];

extern struct object objects[MAX_OBJECTS];
    
extern float cameraYaw;
extern float cameraPitch;
extern float cameraRoll;
extern float cameraX;
extern float cameraY;
extern float cameraZ;

uint32_t* framebuffer;
//int main(int argc, char* argv[]) {
int main() {

    for (size_t i = 0; i<MAX_OBJECTS; i++) {
        objects[i].free = true;
    };

    int woman_id = load_object_from_file("models/woman");
    int box_id = load_object_from_file("models/box");
    move_object(woman_id,0,-1,1);
    move_object(box_id,0,-1,1);
    rotate_object_absolut(woman_id,180,-90,0);
    createTerrain(-10,-1,1,20,5,1,RGB(0,255,0));
    createLine(0,-0.99,1,0,-0.99,5,RGB(255,0,0));
    
    cameraYaw = 0;
    cameraPitch = 0;
    cameraRoll = 0;
    cameraX = 0;
    cameraY = 0;
    cameraZ = 0;
    
    float cameraPos = 0;
    
    load_path_from_file(0,"tracks/camera",-1);
    
    float box_height = 0;
    int box_mode = true;

    uint32_t* framebuffer = outputInit(WINDOW_WIDTH,WINDOW_HEIGHT);
    inputInit();
    
    while (!shouldQuit()) {
        for (size_t i = 0; i<MAX_TRIANGLES; i++) {
            triangles[i].free = true;
        };
        clearBuffer(framebuffer,WINDOW_WIDTH,WINDOW_HEIGHT,0);
        int keyboard = updateInput();

        rotate_object(woman_id,0.5,0,0);
        rotate_object(box_id,-2,0,0);
        move_object_absolut(box_id,0,box_height,1);
        drawObjects();
        moveCamera(0,0,cameraPos);
        if (keyboard&KEY_E) {
            if (keyboard&KEY_W) cameraPos += 0.5f;
            if (keyboard&KEY_S) cameraPos -= 0.5f;
        } else {
            if (keyboard&KEY_W) cameraPos += 0.05f;
            if (keyboard&KEY_S) cameraPos -= 0.05f;
        }
        
        cameraYaw -= getMouseDeltaX() * 100;
        cameraPitch -= getMouseDeltaY() * 100;
        
        
        drawTriangles(framebuffer,cameraX,cameraY,cameraZ,cameraYaw,cameraPitch,cameraRoll,RENDER_MODE_NORMAL);
        
        if (box_mode == true) {
            box_height += 0.01;
            if (box_height >= 1.25) {
                box_mode = false;
            }
        } else {
            box_height -= 0.01;
            if (box_height <= -1.25) {
                box_mode = true;
            }
        }

        framebuffer = flipBuffer();
    }

    cleanup();

    printf("Programm beendet.\n");
    return 0;
}
