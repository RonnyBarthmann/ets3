#define true 1
#define false 0

#include "sdl.h"
#include "render.h"
#include "objects.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern struct triangle triangles[MAX_TRIANGLES];
extern struct triangle triangles_transformed[MAX_TRIANGLES];

extern struct object objects[MAX_OBJECTS];

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
    
    float yaw = 0;
    float pitch = 0;
    float roll = 0;
    float x = 0;
    float y = 0;
    float z = 0;
    
    float box_height = 0;
    int box_mode = true;

    uint32_t* framebuffer = outputInit(WINDOW_WIDTH,WINDOW_HEIGHT);
    inputInit();
    
    while (!shouldQuit()) {
        for (size_t i = 0; i<MAX_TRIANGLES; i++) {
            triangles[i].free = true;
        };
        clearBuffer(framebuffer,WINDOW_WIDTH,WINDOW_HEIGHT,0);
        updateInput();

        //yaw += 1;
        //move_object(woman_id,0,0,0.001);
        rotate_object(woman_id,0.5,0,0);
        rotate_object(box_id,-2,0,0);
        move_object_absolut(box_id,0,box_height,1);
        drawObjects();
        drawTriangles(framebuffer,x,y,z,yaw,pitch,roll);
        
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
