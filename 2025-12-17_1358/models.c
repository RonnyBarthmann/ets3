#include "sdl.h"
#include "render.h"
#include "objects.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "models.h"

extern struct object objects[MAX_OBJECTS];

int createTerrain(float x,float y,float z, float width, float leangth, float tileSize, int color) {
    int terrain_id = create_object();
    if (terrain_id < 0) return -1;
    int tilesZ = leangth / tileSize;
    int tilesX = width / tileSize;
    for (int tileZ=0; tileZ<tilesZ; tileZ++) {
        for (int tileX=0; tileX<tilesX; tileX++) {
            float x1 = x+((float)tileX*tileSize);
            float x2 = x+(((float)tileX+1)*tileSize);
            float z1 = z+((float)tileZ*tileSize);
            float z2 = z+(((float)tileZ+1)*tileSize);
            add_triangle(terrain_id, x1, y, z1, color, x2, y, z1, color, x1, y, z2, color);
            add_triangle(terrain_id, x2, y, z2, color, x2, y, z1, color, x1, y, z2, color);
        }
    }
    return terrain_id;
}

#define LINE_STRENGTH 0.01f

int createLine() {
    int line_id = create_object();
    if (line_id < 0) return -1;
    return line_id;
}

void addToLine(int line_id, float x1,float y1,float z1, float x2, float y2, float z2, int color) {
    if (objects[line_id].free == true) return;
    if (color != -1) {
        add_triangle(line_id,x1,y1,z1,color,x1+LINE_STRENGTH,y1+LINE_STRENGTH,z1+LINE_STRENGTH,color,x2,y2,z2,color);
        add_triangle(line_id,x1,y1,z1,color,x2-LINE_STRENGTH,y2-LINE_STRENGTH,z2-LINE_STRENGTH,color,x2,y2,z2,color);
    }
}

struct pathPoint {
    float x;
    float y;
    float z;
};

#define MAX_PATHS 10
#define MAX_PATH_POINTS 1024

struct pathPoint path[MAX_PATHS][MAX_PATH_POINTS];
int pathLeangth[MAX_PATHS];
int pathLineID[MAX_PATHS];

void createPath(int path_id, float x, float y, float z) {
    if (path_id >= MAX_PATHS) return;
    pathLeangth[path_id] = 0;
    pathLineID[path_id] = createLine();
    path[path_id][0].x = x;
    path[path_id][0].y = y;
    path[path_id][0].z = z;
    pathLeangth[path_id]++;
}

void freePath(int path_id) {
    if (path_id >= MAX_PATHS) return;
    pathLeangth[path_id] = 0;
    objects[pathLineID[path_id]].free = true;
}

void addToPath(int path_id, float x, float y, float z, int color) {
    if (path_id >= MAX_PATHS) return;
    int pos = pathLeangth[path_id];
    if (pos >= MAX_PATH_POINTS) return;
    path[path_id][pos].x = x;
    path[path_id][pos].y = y;
    path[path_id][pos].z = z;
    addToLine(pathLineID[path_id],path[path_id][pos].x,path[path_id][pos].y,path[path_id][pos].z,path[path_id][pos-1].x,path[path_id][pos-1].y,path[path_id][pos-1].z,color);
    pathLeangth[path_id]++;
}

extern float cameraYaw;
extern float cameraPitch;
extern float cameraRoll;
extern float cameraX;
extern float cameraY;
extern float cameraZ;

// Umrechnungsfaktor von Radiant in Grad
static const float DEG2RAD = 180.0f / M_PI;

// Setzt die Kamera so, dass sie auf den Punkt (x,y,z) schaut
// Yaw wird zuerst in der XZ-Ebene berechnet, Pitch danach
// Roll wird immer auf 0 gesetzt
void lookAt(float x, float y, float z)
{
    // Differenzvektor von Kamera zur Zielposition
    float dx = x - cameraX;
    float dy = y - cameraY;
    float dz = z - cameraZ;

    // Zuerst Yaw (Horizontale Drehung in der XZ-Ebene)
    cameraYaw = atan2(dx, dz) * DEG2RAD;  // atan2 gibt Winkel in Radiant → in Grad umrechnen

    // Danach Pitch (Vertikale Neigung)
    // Hypotenuse in XZ-Ebene
    float horizontalDistance = sqrt(dx * dx + dz * dz);
    
    cameraYaw = 360 - cameraYaw;
    
    // Pitch = Winkel zwischen Blickrichtung und XZ-Ebene
    // atan2(dy, horizontalDistance) gibt korrekte Pitch (negativ = nach unten)
    cameraPitch = 0 - atan2(dy, horizontalDistance) * DEG2RAD;

    // Roll wird immer auf 0 gesetzt
    cameraRoll = 0.0f;
}

// Hauptfunktion: Bewegt die Kamera entlang eines Pfades
void moveCamera(int path_id, int direction, float position) {
    int len = pathLeangth[path_id];
    
    // Sonderfall: Pfad leer (Länge 0)
    if (len == 0) {
        cameraX = cameraY = cameraZ = 0.0f;
        cameraYaw = cameraPitch = cameraRoll = 0.0f;
        return;
    }

    // Sonderfall: Pfad hat nur 1 Punkt
    if (len == 1) {
        cameraX = path[path_id][0].x;
        cameraY = path[path_id][0].y;
        cameraZ = path[path_id][0].z;
        cameraYaw = cameraPitch = cameraRoll = 0.0f;
        return;
    }

    // Normalfall: len >= 2, Pfad ist geschlossen (looping)

    // Schritt 1: Position normalisieren (modulo Gesamtlänge)
    // Zuerst Gesamtlänge des Pfades berechnen
    float totalLength = 0.0f;
    for (int i = 0; i < len; i++) {
        int next = (i + 1) % len; // schließt den Kreis
        float dx = path[path_id][next].x - path[path_id][i].x;
        float dy = path[path_id][next].y - path[path_id][i].y;
        float dz = path[path_id][next].z - path[path_id][i].z;
        totalLength += sqrtf(dx*dx + dy*dy + dz*dz);
    }

    // Position modulo Gesamtlänge (kann negativ sein → positiv machen)
    float pos = fmodf(position, totalLength);
    if (pos < 0) pos += totalLength;

    // Richtung umkehren, wenn direction == 1
    if (direction == 1) {
        pos = totalLength - pos;
    }

    // Schritt 2: Finde die Position auf dem Pfad (interpoliert)
    float currentDistance = 0.0f;
    int segmentStart = 0;
    float segmentLength = 0.0f;

    for (int i = 0; i < len; i++) {
        int next = (i + 1) % len;
        float dx = path[path_id][next].x - path[path_id][i].x;
        float dy = path[path_id][next].y - path[path_id][i].y;
        float dz = path[path_id][next].z - path[path_id][i].z;
        segmentLength = sqrtf(dx*dx + dy*dy + dz*dz);

        if (currentDistance + segmentLength >= pos) {
            // Wir liegen in diesem Segment
            segmentStart = i;
            break;
        }
        currentDistance += segmentLength;
    }

    // Interpolation innerhalb des Segments
    float t = (pos - currentDistance) / segmentLength;
    int next = (segmentStart + 1) % len;

    float camX = path[path_id][segmentStart].x + t * (path[path_id][next].x - path[path_id][segmentStart].x);
    float camY = path[path_id][segmentStart].y + t * (path[path_id][next].y - path[path_id][segmentStart].y);
    float camZ = path[path_id][segmentStart].z + t * (path[path_id][next].z - path[path_id][segmentStart].z);

    // Kamera positionieren
    cameraX = camX;
    cameraY = camY;
    cameraZ = camZ;

    // Richtung bestimmen: schaut zum nächsten Punkt (direction == 0) oder vorherigen (direction == 1)
    int targetIndex;
    if (direction == 0) {
        targetIndex = (segmentStart + 1) % len;
    } else {
        targetIndex = (segmentStart + len - 1) % len; // vorheriger Punkt
    }

    lookAt(path[path_id][targetIndex].x,
           path[path_id][targetIndex].y,
           path[path_id][targetIndex].z);
}

int load_path_from_file(int path_id, const char* filename, int color) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Fehler: Konnte Datei '%s' nicht öffnen\n", filename);
        return -1;
    }

    float x, y, z;
    int first_point = 1;
    char line[16384];

    while (fgets(line, sizeof(line), file)) {
        // Überspringe leere Zeilen und Kommentare (falls gewünscht)
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '#') {
            continue;
        }

        // Versuche, drei Floats zu parsen (x,y,z)
        if (sscanf(line, "%f,%f,%f", &x, &y, &z) == 3) {
            if (first_point) {
                // Erster Punkt → Pfad neu erstellen
                createPath(path_id, x, y, z);
                first_point = 0;
            } else {
                // Alle weiteren Punkte mit Farbe hinzufügen
                addToPath(path_id, x, y, z, color);
            }
        } else {
            // Ungültiges Format → Fehlermeldung und Abbruch
            fprintf(stderr, "Fehler: Ungültiges Format in Zeile '%s' in Datei '%s'\n", line, filename);
            fclose(file);
            return -1;
        }
    }

    fclose(file);

    // Optional: Prüfen, ob überhaupt ein Punkt geladen wurde
    if (first_point) {
        fprintf(stderr, "Warnung: Datei '%s' enthielt keine gültigen Punkte\n", filename);
        return -1;
    }

    return path_id;  // Erfolg
}
