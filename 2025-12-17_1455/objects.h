#define true 1
#define false 0

#ifndef OBJECTS_H
#define OBJECTS

#define MAX_OBJECT_TRIANGLES 100000
#define MAX_OBJECTS 1024

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

void drawObjects();
int load_object_from_file(const char* filename);
int create_object();
void add_triangle(int object_id, float x1, float y1, float z1, uint32_t c1, float x2, float y2, float z2, uint32_t c2, float x3, float y3, float z3, uint32_t c3);
void move_object(int id,float x, float y, float z);
void move_object_absolut(int id,float x, float y, float z);
void rotate_object(int id,float yaw, float pitch, float roll);
void rotate_object_absolut(int id,float yaw, float pitch, float roll);

#endif // OBJECTS_H
