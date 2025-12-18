#define true 1
#define false 0

#ifndef RENDER_H
#define RENDER_H

#define WINDOW_WIDTH 2560
#define WINDOW_HEIGHT 1440

#define RENDER_MODE_NORMAL 0
#define RENDER_MODE_WIREFRAME 1
#define RENDER_MODE_POINTS 2

#define MAX_TRIANGLES 1048576

struct triangle {
    int free;
    float x1, y1, z1;
    uint32_t c1;
    float x2, y2, z2;
    uint32_t c2;
    float x3, y3, z3;
    uint32_t c3;
};

void transformTriangles(float x, float y, float z, float yaw, float pitch, float roll);

uint32_t RGB(int r,int g, int b);
void drawTriangle(uint32_t* buffer, int bufferWidth, int bufferHeight,
                 float x1, float y1, float z1, uint32_t c1,
                 float x2, float y2, float z2, uint32_t c2,
                 float x3, float y3, float z3, uint32_t c3);
void drawTriangleWireframe(uint32_t* buffer, int bufferWidth, int bufferHeight,
                           float x1, float y1, float z1, uint32_t c1,
                           float x2, float y2, float z2, uint32_t c2,
                           float x3, float y3, float z3, uint32_t c3);
void drawTriangles(uint32_t* framebuffer, float x, float y, float z, float yaw, float pitch, float roll, int mode);
void clearBuffer(uint32_t* buffer, int bufferWidth, int bufferHeight, uint32_t color);

#endif // RENDER_H
