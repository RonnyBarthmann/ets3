void createPath(int path_id, float x, float y, float z) {
    if (path_id >= MAX_PATHS || path_id < 0) return;
    pathLeangth[path_id] = 0;
    pathLineID[path_id] = createLine();
    path[path_id][0].x = x;
    path[path_id][0].y = y;
    path[path_id][0].z = z;
    pathLeangth[path_id]++;
}

void freePath(int path_id) {
    if (path_id >= MAX_PATHS || path_id < 0) return;
    pathLeangth[path_id] = 0;
    objects[pathLineID[path_id]].free = true;
}

void addToPath(int path_id, float x, float y, float z, int color) {
    if (path_id >= MAX_PATHS || path_id < 0) return;
    int pos = pathLeangth[path_id];
    if (pos >= MAX_PATH_POINTS) return;
    path[path_id][pos].x = x;
    path[path_id][pos].y = y;
    path[path_id][pos].z = z;
    addToLine(pathLineID[path_id],path[path_id][pos].x,path[path_id][pos].y,path[path_id][pos].z,path[path_id][pos-1].x,path[path_id][pos-1].y,path[path_id][pos-1].z,color);
    pathLeangth[path_id]++;
}

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
    cameraYaw = 360 - (atan2(dx, dz) * DEG2RAD);  // atan2 gibt Winkel in Radiant → in Grad umrechnen

    // Danach Pitch (Vertikale Neigung)
    // Hypotenuse in XZ-Ebene
    float horizontalDistance = sqrt(dx * dx + dz * dz);
    
    // Pitch = Winkel zwischen Blickrichtung und XZ-Ebene
    // atan2(dy, horizontalDistance) gibt korrekte Pitch (negativ = nach unten)
    cameraPitch = 0 - atan2(dy, horizontalDistance) * DEG2RAD;

    // Roll wird immer auf 0 gesetzt
    cameraRoll = 0.0f;
}
void objectLookAt(int object_id, float x, float y, float z) {
    if (object_id >= MAX_OBJECTS || object_id < 0) return;
    if (objects[object_id].free == true) return;
    // Differenzvektor von Kamera zur Zielposition
    float dx = x - objects[object_id].x;
    float dy = y - objects[object_id].y;
    float dz = z - objects[object_id].z;

    // Zuerst Yaw (Horizontale Drehung in der XZ-Ebene)
    objects[object_id].yaw = (atan2(dx, dz) * DEG2RAD);  // atan2 gibt Winkel in Radiant → in Grad umrechnen

    // Danach Pitch (Vertikale Neigung)
    // Hypotenuse in XZ-Ebene
    float horizontalDistance = sqrt(dx * dx + dz * dz);
    
    // Pitch = Winkel zwischen Blickrichtung und XZ-Ebene
    // atan2(dy, horizontalDistance) gibt korrekte Pitch (negativ = nach unten)
    objects[object_id].pitch = 0 - atan2(dy, horizontalDistance) * DEG2RAD;

    // Roll wird immer auf 0 gesetzt
    objects[object_id].roll = 0.0f;
}

// Hauptfunktion: Bewegt ein Objekt entlang eines Pfades
void moveObjectOnPath(int object_id, int path_id, int direction, float position, int rotate) {
    if (path_id >= MAX_PATHS || path_id < 0) return;
    if (object_id >= MAX_OBJECTS || object_id < 0) return;
    if (objects[object_id].free == true) return;
    int len = pathLeangth[path_id];
    
    
    // Sonderfall: Pfad leer (Länge 0)
    if (len == 0) {
        objects[object_id].x = objects[object_id].y = objects[object_id].z = 0.0f;
        if (rotate == true) {
            objects[object_id].yaw = objects[object_id].pitch = objects[object_id].roll = 0.0f;
        }
        return;
    }

    // Sonderfall: Pfad hat nur 1 Punkt
    if (len == 1) {
        objects[object_id].x = path[path_id][0].x;
        objects[object_id].y = path[path_id][0].y;
        objects[object_id].z = path[path_id][0].z;
        if (rotate == true) {
            objects[object_id].yaw = objects[object_id].pitch = objects[object_id].roll = 0.0f;
        }
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
    objects[object_id].x = camX;
    objects[object_id].y = camY;
    objects[object_id].z = camZ;

    if (rotate == true) {
        // Richtung bestimmen: schaut zum nächsten Punkt (direction == 0) oder vorherigen (direction == 1)
        int targetIndex;
        if (direction == 0) {
            targetIndex = (segmentStart + 1) % len;
        } else {
            targetIndex = (segmentStart + len - 1) % len; // vorheriger Punkt
        }
        
        objectLookAt(object_id,path[path_id][targetIndex].x,
            path[path_id][targetIndex].y,
            path[path_id][targetIndex].z);
    }
}

int load_path_from_file(int path_id, const char* filename, int color) {
    if (path_id >= MAX_PATHS || path_id < 0) return -1;
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
