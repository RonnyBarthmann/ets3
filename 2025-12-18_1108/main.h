#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "sdl.h"

#define true 1
#define false 0

#define WINDOW_WIDTH 2560
#define WINDOW_HEIGHT 1440

#define RENDER_MODE_NORMAL 0
#define RENDER_MODE_WIREFRAME 1
#define RENDER_MODE_POINTS 2

#define MAX_TRIANGLES 1048576 

#define MAX_OBJECT_TRIANGLES 100000
#define MAX_OBJECTS 1024

#define LINE_STRENGTH 0.01f

#define MAX_PATHS 10
#define MAX_PATH_POINTS 1024

struct triangle {
    int free;
    float x1, y1, z1;
    uint32_t c1;
    float x2, y2, z2;
    uint32_t c2;
    float x3, y3, z3;
    uint32_t c3;
};

struct object {
    int free;
    float x, y, z, yaw, pitch, roll;
    float x1[MAX_OBJECT_TRIANGLES];
    float y1[MAX_OBJECT_TRIANGLES];
    float z1[MAX_OBJECT_TRIANGLES];
    uint32_t c1[MAX_OBJECT_TRIANGLES];
    float x2[MAX_OBJECT_TRIANGLES];
    float y2[MAX_OBJECT_TRIANGLES];
    float z2[MAX_OBJECT_TRIANGLES];
    uint32_t c2[MAX_OBJECT_TRIANGLES];
    float x3[MAX_OBJECT_TRIANGLES];
    float y3[MAX_OBJECT_TRIANGLES];
    float z3[MAX_OBJECT_TRIANGLES];
    uint32_t c3[MAX_OBJECT_TRIANGLES];
    int triangles_count;
};

struct pathPoint {
    float x;
    float y;
    float z;
};

struct object objects[MAX_OBJECTS];
struct triangle triangles[MAX_TRIANGLES];
struct triangle triangles_transformed[MAX_TRIANGLES];
struct pathPoint path[MAX_PATHS][MAX_PATH_POINTS];
float zBuffer[128*1048576];
int pathLeangth[MAX_PATHS];
int pathLineID[MAX_PATHS];

float cameraDistance;
float cameraYaw;
float cameraPitch;
float cameraRoll;
float cameraX;
float cameraY;
float cameraZ;

// Umrechnungsfaktor von Radiant in Grad
const float DEG2RAD = 180.0f / M_PI;


// Misc
void rotatePoint(float x, float y, float z, float yaw_deg, float pitch_deg, float roll_deg, float *out_x, float *out_y, float *out_z);
void rotatePoint_reverse(float x, float y, float z, float yaw_deg, float pitch_deg, float roll_deg, float *out_x, float *out_y, float *out_z);

// Objects
void drawObjects();
int load_object_from_file(const char* filename);
int create_object();
void move_object(int id,float x, float y, float z);
void move_object_absolut(int id,float x, float y, float z);
void rotate_object(int id,float yaw, float pitch, float roll);
void rotate_object_absolut(int id,float yaw, float pitch, float roll);

// Paths
int createTerrain(float x,float y,float z, float width, float leangth, float tileSize, int color);
int createLine();
void addToLine(int line_id, float x1,float y1,float z1, float x2, float y2, float z2, int color);
void createPath(int path_id, float x, float y, float z);
void freePath(int path_id);
void addToPath(int path_id, float x, float y, float z, int color);
void lookAt(float targetX, float targetY, float targetZ);
void objectLookAt(int object_id, float targetX, float targetY, float targetZ);
void moveObjectOnPath(int object_id, int path_id, int direction, float position, int rotate);
int load_path_from_file(int path_id, const char* filename, int color);

// Renderer
void add_triangle(int object_id, float x1, float y1, float z1, uint32_t c1, float x2, float y2, float z2, uint32_t c2, float x3, float y3, float z3, uint32_t c3);
void transformTriangles(float x, float y, float z, float yaw, float pitch, float roll);
uint32_t RGB(int r,int g, int b);
void drawTriangle(uint32_t* buffer, int bufferWidth, int bufferHeight, float x1, float y1, float z1, uint32_t c1, float x2, float y2, float z2, uint32_t c2, float x3, float y3, float z3, uint32_t c3);
void drawTriangleWireframe(uint32_t* buffer, int bufferWidth, int bufferHeight, float x1, float y1, float z1, uint32_t c1, float x2, float y2, float z2, uint32_t c2, float x3, float y3, float z3, uint32_t c3);
void drawTriangles(uint32_t* framebuffer, float x, float y, float z, float yaw, float pitch, float roll, int mode);
void clearBuffer(uint32_t* buffer, int bufferWidth, int bufferHeight, uint32_t color);
