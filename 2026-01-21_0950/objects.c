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
        
        // Überspringe leere Zeilen und Kommentare (falls gewünscht)
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '#') {
            continue;
        }

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
        
        if (slot >= MAX_OBJECT_TRIANGLES) break;
    }

    fclose(file);
    return object_id;
}

int create_object() {
    
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
    
    return object_id;
}

void add_triangle(int object_id, float x1, float y1, float z1, uint32_t c1, float x2, float y2, float z2, uint32_t c2, float x3, float y3, float z3, uint32_t c3) {
    if (object_id >= MAX_OBJECTS || object_id < 0) return;
    if (objects[object_id].free == false) {
        int triangle = objects[object_id].triangles_count;
        if (triangle < MAX_OBJECT_TRIANGLES) {
            objects[object_id].x1[triangle] = x1;
            objects[object_id].y1[triangle] = y1;
            objects[object_id].z1[triangle] = z1;
            objects[object_id].c1[triangle] = c1;
            objects[object_id].x2[triangle] = x2;
            objects[object_id].y2[triangle] = y2;
            objects[object_id].z2[triangle] = z2;
            objects[object_id].c2[triangle] = c2;
            objects[object_id].x3[triangle] = x3;
            objects[object_id].y3[triangle] = y3;
            objects[object_id].z3[triangle] = z3;
            objects[object_id].c3[triangle] = c3;
            objects[object_id].triangles_count++;
        }
    }
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
                float yaw = objects[i].yaw;
                float pitch = objects[i].pitch;
                float roll = objects[i].roll;
                    
                rotatePoint_reverse(x1,y1,z1,yaw,pitch,roll,&x1,&y1,&z1);
                rotatePoint_reverse(x2,y2,z2,yaw,pitch,roll,&x2,&y2,&z2);
                rotatePoint_reverse(x3,y3,z3,yaw,pitch,roll,&x3,&y3,&z3);
                
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
    if (id < MAX_OBJECTS && id >= 0) {
        if (objects[id].free == false) {
            objects[id].x += x;
            objects[id].y += y;
            objects[id].z += z;
        }
    }
}

void move_object_absolut(int id,float x, float y, float z) {
    if (id < MAX_OBJECTS && id >= 0) {
        if (objects[id].free == false) {
            objects[id].x = x;
            objects[id].y = y;
            objects[id].z = z;
        }
    }
}

void rotate_object(int id,float yaw, float pitch, float roll) {
    if (id < MAX_OBJECTS && id >= 0) {
        if (objects[id].free == false) {
            objects[id].yaw += yaw;
            objects[id].pitch += pitch;
            objects[id].roll += roll;
        }
    }
}

void rotate_object_absolut(int id,float yaw, float pitch, float roll) {
    if (id < MAX_OBJECTS && id >= 0) {
        if (objects[id].free == false) {
            objects[id].yaw = yaw;
            objects[id].pitch = pitch;
            objects[id].roll = roll;
        }
    }
}

void syncCameraToObject(int object_id, int rotate) {
    if (object_id >= MAX_OBJECTS || object_id < 0) return;
    if (objects[object_id].free == true) return;
    cameraX = objects[object_id].x;
    cameraY = objects[object_id].y;
    cameraZ = objects[object_id].z;
    if (rotate == true) {
        cameraYaw = 360 - objects[object_id].yaw;
        cameraPitch = 360 - objects[object_id].pitch;
        cameraRoll = 360 - objects[object_id].roll;
    }
}
