#include "main.h"
#include "misc.c"
#include "render.c"
#include "models.c"
#include "objects.c"
#include "path.c"


uint32_t* framebuffer;
//int main(int argc, char* argv[]) {
int main() {

    for (size_t i = 0; i<MAX_OBJECTS; i++) {
        objects[i].free = true;
    };
    
    int chaos_id = create_object();
    int chaos2_id = create_object();
    float chaos_x = 0;
    float chaos_y = 0;
    float chaos_z = 0;
    float chaos_s = 5;
    float chaos_i = 100;
    for (int i=0; i<chaos_i; i++) {
        add_triangle(chaos_id,
                     rand_min_max(-chaos_s,chaos_s)+chaos_x,rand_min_max(-chaos_s,chaos_s)+chaos_y,rand_min_max(-chaos_s,chaos_s)+chaos_z,random_max(16777215),
                     rand_min_max(-chaos_s,chaos_s)+chaos_x,rand_min_max(-chaos_s,chaos_s)+chaos_y,rand_min_max(-chaos_s,chaos_s)+chaos_z,random_max(16777215),
                     rand_min_max(-chaos_s,chaos_s)+chaos_x,rand_min_max(-chaos_s,chaos_s)+chaos_y,rand_min_max(-chaos_s,chaos_s)+chaos_z,random_max(16777215));
        add_triangle(chaos2_id,
                     rand_min_max(-chaos_s,chaos_s)+chaos_x,rand_min_max(-chaos_s,chaos_s)+chaos_y,rand_min_max(-chaos_s,chaos_s)+chaos_z,random_max(16777215),
                     rand_min_max(-chaos_s,chaos_s)+chaos_x,rand_min_max(-chaos_s,chaos_s)+chaos_y,rand_min_max(-chaos_s,chaos_s)+chaos_z,random_max(16777215),
                     rand_min_max(-chaos_s,chaos_s)+chaos_x,rand_min_max(-chaos_s,chaos_s)+chaos_y,rand_min_max(-chaos_s,chaos_s)+chaos_z,random_max(16777215));
    }
    move_object_absolut(chaos_id,0,0,110);

    int woman_id = load_object_from_file("models/woman");
    int woman2_id = load_object_from_file("models/woman");
    int box_id = load_object_from_file("models/box");
    int cab_id = load_object_from_file("models/cab");
    move_object(woman_id,0,-1,1);
    move_object(woman2_id,0,-1,-10);
    move_object(box_id,0,-1,1);
    createTerrain(-10,-1,1,20,5,2.5,RGB(0,255,0));
    createLine(0,-0.99,1,0,-0.99,5,RGB(255,0,0));
    
    cameraDistance = 100;
    cameraYaw = 0;
    cameraPitch = 0;
    cameraRoll = 0;
    cameraX = 0;
    cameraY = 0;
    cameraZ = 0;
    int cameraMode = RENDER_MODE_NORMAL;
    
    load_path_from_file(0,"tracks/camera",-1);
    load_path_from_file(1,"tracks/circle50y",-1);
    
    float box_height = 0;
    int box_mode = true;
    
    float speed = 0;
    float old_time = 0;

    uint32_t* framebuffer = outputInit(WINDOW_WIDTH,WINDOW_HEIGHT);
    inputInit();
    timerInit();
        
    int syncToCab = false;
    
    while (!shouldQuit()) {
        for (size_t i = 0; i<MAX_TRIANGLES; i++) {
            triangles[i].free = true;
        };
        clearBuffer(framebuffer,WINDOW_WIDTH,WINDOW_HEIGHT,0);
        int keyboard = updateInput();

        float time = (float)get_ms_since_start()/1000;
        float delay = time - old_time;
        old_time = time;
        printf("%f",1/delay);
            
        rotate_object_absolut(woman_id,0,-90,time*10);
        rotate_object_absolut(woman2_id,0,-90,0);
        
        rotate_object_absolut(box_id,time*-50,0,0);
        move_object_absolut(box_id,0,box_height,1);
        
        moveObjectOnPath(cab_id,0,0,time*2,1);
        moveObjectOnPath(chaos2_id,1,0,time*50,0);
        //rotate_object(cab_id,0,0,time*-5);
        
        for (int i=0; i<objects[chaos_id].triangles_count; i++) {
            objects[chaos_id].x1[i] += rand_min_max(-0.1,0.1);
            objects[chaos_id].y1[i] += rand_min_max(-0.1,0.1);
            objects[chaos_id].z1[i] += rand_min_max(-0.1,0.1);
            objects[chaos_id].x2[i] += rand_min_max(-0.1,0.1);
            objects[chaos_id].y2[i] += rand_min_max(-0.1,0.1);
            objects[chaos_id].z2[i] += rand_min_max(-0.1,0.1);
            objects[chaos_id].x3[i] += rand_min_max(-0.1,0.1);
            objects[chaos_id].y3[i] += rand_min_max(-0.1,0.1);
            objects[chaos_id].z3[i] += rand_min_max(-0.1,0.1);
        }

        cameraYaw -= getMouseDeltaX() * 2;
        cameraPitch -= getMouseDeltaY() * 2;
        
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
            syncCameraToObject(cab_id,true);
            syncToCab = true;
        }
        if (keyboard&KEY_RETURN) {
            syncToCab = false;
        }
        if (syncToCab == true) {
            syncCameraToObject(cab_id,false);
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
            syncToCab = false;
        }
        
        drawObjects();
        drawTriangles(framebuffer,cameraX,cameraY,cameraZ,cameraYaw,cameraPitch,cameraRoll,cameraMode);
        
        if (box_mode == true) {
            box_height += delay;
            if (box_height >= 1.25) {
                box_mode = false;
            }
        } else {
            box_height -= delay;
            if (box_height <= -1.25) {
                box_mode = true;
            }
        }

        printf("\n",1/delay);
        framebuffer = flipBuffer();
    }

    cleanup();

    printf("Programm beendet.\n");
    return 0;
}
