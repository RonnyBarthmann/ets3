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
    int woman2_id = load_object_from_file("models/woman");
    int box_id = load_object_from_file("models/box");
    move_object(woman_id,0,-1,1);
    move_object(woman2_id,0,-1,-10);
    rotate_object_absolut(woman2_id,0,-90,0);
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
    
    load_path_from_file(0,"tracks/camera",-1);
    
    float box_height = 0;
    int box_mode = true;
    
    float speed = 0;

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

        cameraYaw -= getMouseDeltaX() * 2;
        cameraPitch -= getMouseDeltaY() * 2;
        float yawRad = cameraYaw * (M_PI / 180.0f);
        float forward1 = sinf(yawRad);
        float forward2 = cosf(yawRad);
        
        if (keyboard&KEY_E) {
            speed = 0.5f;
        } else {
            speed = 0.05f;
        }
        if (keyboard&KEY_W) {
            cameraX -= forward1*speed;
            cameraZ += forward2*speed;
        }
        if (keyboard&KEY_S) {
            cameraX += forward1*speed;
            cameraZ -= forward2*speed;
        }
        if (keyboard&KEY_A) {
            cameraX -= forward2*speed;
            cameraZ -= forward1*speed;
        }
        if (keyboard&KEY_D) {
            cameraX += forward2*speed;
            cameraZ += forward1*speed;
        }
        if (keyboard&KEY_SHIFT) {
            cameraY += speed;
        }
        if (keyboard&KEY_Q) {
            cameraY -= speed;
        }
        
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
