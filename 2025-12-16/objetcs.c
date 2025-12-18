#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "objects.h"
#include "render.h"

extern struct triangle triangles[MAX_TRIANGLES];
extern struct triangle triangles_transformed[MAX_TRIANGLES];

struct object objects[MAX_OBJECTS];

int load_object_from_file(const char* filename) {
    
    // Suche nach einem freien Slot im Array
    int object_id = -1;
    for (int i = 0; i < MAX_OBJECTS; i++) {
        if (objects[i].free == true) {  // freier Platz gefunden
            object_id = i;
            break;
        }
    }
    // Wenn kein freier Platz mehr vorhanden ist → abbrechen
    if (object_id == -1) {
        printf("Maximum an Objekten (%d) erreicht.\n", MAX_OBJECTS);
        return -1;
    }
    
    objects[object_id].free = false;
    objects[object_id].x = 0;
    objects[object_id].y = 0;
    objects[object_id].z = 0;
    objects[object_id].yaw = 0;
    objects[object_id].pitch = 0;
    objects[object_id].roll = 0;
    objects[object_id].triangles_count = 0;
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Konnte Datei nicht öffnen");
        return -1;
    }

    char line[16384];
    int slot = 0;
    
    while (fgets(line, sizeof(line), file)) {
        // Zeilenende-Zeichen entfernen
        line[strcspn(line, "\r\n")] = 0;

        // Parsen der 12 Werte: x1,y1,z1,c1, x2,y2,z2,c2, x3,y3,z3,c3
        float x1,y1,z1, x2,y2,z2, x3,y3,z3;
        int c1, c2, c3;

        int parsed = sscanf(line, "%f,%f,%f,%d,%f,%f,%f,%d,%f,%f,%f,%d",
                            &x1, &y1, &z1, &c1,
                            &x2, &y2, &z2, &c2,
                            &x3, &y3, &z3, &c3);

        if (parsed != 12) {
            fprintf(stderr, "Fehler beim Parsen der Zeile: %s\n", line);
            continue;  // Zeile überspringen
        }

        // Eintrag in den gefundenen Slot schreiben
        objects[object_id].x1[slot] = x1;
        objects[object_id].y1[slot] = y1;
        objects[object_id].z1[slot] = z1;
        objects[object_id].c1[slot] = c1;
        objects[object_id].x2[slot] = x2;
        objects[object_id].y2[slot] = y2;
        objects[object_id].z2[slot] = z2;
        objects[object_id].c2[slot] = c2;
        objects[object_id].x3[slot] = x3;
        objects[object_id].y3[slot] = y3;
        objects[object_id].z3[slot] = z3;
        objects[object_id].c3[slot] = c3;
        slot++;
        objects[object_id].triangles_count = slot;
    }

    fclose(file);
    return object_id;
}

static void rotatePoint(float x, float y, float z,
                        float yaw, float pitch, float roll,
                        float *out_x, float *out_y, float *out_z)
{
    float sx = sinf(roll);
    float cx = cosf(roll);
    float sp = sinf(pitch);
    float cp = cosf(pitch);
    float sr = sinf(yaw);
    float cr = cosf(yaw);

    // 1. Yaw (yaw) um Y-Achse
    float temp_x = x * cx - y * sx;
    float temp_y = x * sx + y * cx;
    float temp_z = z;

    // 2. Pitch um X-Achse
    float temp_y2 = temp_y * cp - temp_z * sp;
    float temp_z2 = temp_y * sp + temp_z * cp;
    temp_y = temp_y2;
    temp_z = temp_z2;

    // 3. Roll um Z-Achse
    float final_x = temp_z * sr + temp_x * cr;
    float final_z = temp_z * cr - temp_x * sr;
    float final_y = temp_y;

    *out_x = final_x;
    *out_y = final_y;
    *out_z = final_z;
}

void drawObjects() {
    for (int i = 0; i < MAX_OBJECTS; ++i)
    {
        if (objects[i].free == false) {
            for (int source = 0; source < objects[i].triangles_count; source++) {
                // Suche nach einem freien Slot im Array
                int dest = -1;
                for (int i2 = 0; i2 < MAX_TRIANGLES; i2++) {
                    if (triangles[i2].free == true) {  // freier Platz gefunden
                        dest = i2;
                        break;
                    }
                }
                // Wenn kein freier Platz mehr vorhanden ist → abbrechen
                if (dest == -1) {
                    return;
                }
                
                triangles[dest].free = false;
                float x1 = objects[i].x1[source];
                float y1 = objects[i].y1[source];
                float z1 = objects[i].z1[source];
                float x2 = objects[i].x2[source];
                float y2 = objects[i].y2[source];
                float z2 = objects[i].z2[source];
                float x3 = objects[i].x3[source];
                float y3 = objects[i].y3[source];
                float z3 = objects[i].z3[source];
                float yaw = objects[i].yaw * (M_PI / 180.0f);
                float pitch = objects[i].pitch * (M_PI / 180.0f);
                float roll = objects[i].roll* (M_PI / 180.0f);
                    
                rotatePoint(x1,y1,z1,yaw,pitch,roll,&x1,&y1,&z1);
                rotatePoint(x2,y2,z2,yaw,pitch,roll,&x2,&y2,&z2);
                rotatePoint(x3,y3,z3,yaw,pitch,roll,&x3,&y3,&z3);
                
                triangles[dest].x1 = x1 + objects[i].x;
                triangles[dest].y1 = y1 + objects[i].y;
                triangles[dest].z1 = z1 + objects[i].z;
                triangles[dest].c1 = objects[i].c1[source];
                triangles[dest].x2 = x2 + objects[i].x;
                triangles[dest].y2 = y2 + objects[i].y;
                triangles[dest].z2 = z2 + objects[i].z;
                triangles[dest].c2 = objects[i].c2[source];
                triangles[dest].x3 = x3 + objects[i].x;
                triangles[dest].y3 = y3 + objects[i].y;
                triangles[dest].z3 = z3 + objects[i].z;
                triangles[dest].c3 = objects[i].c3[source];
            }
        }
    }
}

void move_object(int id,float x, float y, float z) {
    if (id < MAX_OBJECTS) {
        if (objects[id].free == false) {
            objects[id].x += x;
            objects[id].y += y;
            objects[id].z += z;
        }
    }
}

void move_object_absolut(int id,float x, float y, float z) {
    if (id < MAX_OBJECTS) {
        if (objects[id].free == false) {
            objects[id].x = x;
            objects[id].y = y;
            objects[id].z = z;
        }
    }
}

void rotate_object(int id,float yaw, float pitch, float roll) {
    if (id < MAX_OBJECTS) {
        if (objects[id].free == false) {
            objects[id].yaw += yaw;
            objects[id].pitch += pitch;
            objects[id].roll += roll;
        }
    }
}

void rotate_object_absolut(int id,float yaw, float pitch, float roll) {
    if (id < MAX_OBJECTS) {
        if (objects[id].free == false) {
            objects[id].yaw = yaw;
            objects[id].pitch = pitch;
            objects[id].roll = roll;
        }
    }
}
