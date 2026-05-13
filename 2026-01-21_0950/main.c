#include "main.h"
//#include "memory.c"
#include "misc.c"
#include "render.c"
#include "models.c"
#include "objects.c"
#include "path.c"
#include "lua.c"
//#include "chunks.c"

//int main(int argc, char* argv[]) {
int main() {

/*
    for (size_t i = 0; i<MAX_OBJECTS; i++) {
        objects[i].free = true;
    };
    
    int chaos_id = create_object();
    float chaos_x = 0;
    float chaos_y = 0;
    float chaos_z = 0;
    float chaos_s = 5;
    float chaos_i = 10;
    for (int i=0; i<chaos_i; i++) {
        add_triangle(chaos_id,
                     rand_min_max(-chaos_s,chaos_s)+chaos_x,rand_min_max(-chaos_s,chaos_s)+chaos_y,rand_min_max(-chaos_s,chaos_s)+chaos_z,random_max(16777215),
                     rand_min_max(-chaos_s,chaos_s)+chaos_x,rand_min_max(-chaos_s,chaos_s)+chaos_y,rand_min_max(-chaos_s,chaos_s)+chaos_z,random_max(16777215),
                     rand_min_max(-chaos_s,chaos_s)+chaos_x,rand_min_max(-chaos_s,chaos_s)+chaos_y,rand_min_max(-chaos_s,chaos_s)+chaos_z,random_max(16777215));
    }
    move_object_absolut(chaos_id,0,0,10);
    
    cameraSpeed = 2;
    cameraDistance = 100;
    cameraYaw = 0;
    cameraPitch = 0;
    cameraRoll = 0;
    cameraX = 0;
    cameraY = 0;
    cameraZ = 0;
    cameraMode = RENDER_MODE_NORMAL;
    
    float speed = 0;
    float old_time = 0;

    uint32_t* framebuffer = outputInit(WINDOW_WIDTH,WINDOW_HEIGHT);
    inputInit();
    timerInit();
*/

    L = InitLUA();
    DoLUA(L);

    //lua_init(0);
    //while (!shouldQuit()) lua_step(0);
    
    cleanup();

    printf("C Programm beendet.\n");
    return 0;

/*
    // Init World
//    initChunks();
    
    while (!shouldQuit()) {
        // clear Scene
        for (size_t i = 0; i<MAX_TRIANGLES; i++) {
            triangles[i].free = true;
        };
        clearBuffer(framebuffer,WINDOW_WIDTH,WINDOW_HEIGHT,0);
        
        // Update Input
        int keyboard = updateInput();

        // Print FPS
        float time = (float)get_ms_since_start()/1000;
        float delay = time - old_time;
        old_time = time;
        printf("%f",1/delay);

        // Handle Inputs
        cameraYaw -= getMouseDeltaX() * cameraSpeed;
        cameraPitch -= getMouseDeltaY() * cameraSpeed;
        
        float yawRad = cameraYaw * (M_PI / 180.0f);
        float forward1 = sinf(yawRad);
        float forward2 = cosf(yawRad);
        
        if (keyboard&KEY_E) {
            speed = delay*5.0f;
        } else {
            speed = delay*0.5f;
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
        if (keyboard&KEY_SPACE) {
        }
        if (keyboard&KEY_RETURN) {
        }
        if (keyboard&KEY_LEFT) {
            cameraMode = RENDER_MODE_NORMAL;
        }
        if (keyboard&KEY_RIGHT) {
            cameraMode = RENDER_MODE_WIREFRAME;
        }
        if (keyboard&KEY_UP) {
            cameraDistance += 0.1;
            printf("  %f",cameraDistance);
        }
        if (keyboard&KEY_DOWN) {
            cameraDistance -= 0.1;
            printf("  %f",cameraDistance);
        }
        if (keyboard&KEY_CTRL) {
            cameraYaw = 0;
            cameraPitch = 0;
            cameraRoll = 0;
            cameraX = 0;
            cameraY = 0;
            cameraZ = 0;
        }
        
        // Update World
//        updateChunks();
        
        // Render
        drawObjects();
        drawTriangles(framebuffer,cameraX,cameraY,cameraZ,cameraYaw,cameraPitch,cameraRoll,cameraMode);

        printf("\n",1/delay);
        framebuffer = flipBuffer();
    }
    
*/

    cleanup();

    printf("Programm beendet.\n");
    return 0;
}
