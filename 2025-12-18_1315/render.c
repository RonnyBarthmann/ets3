// Hilfsfunktion: Rotation um die Y-Achse (Yaw)
static void rotate_yaw(float x, float y, float z, float yaw_deg, float *out_x, float *out_y, float *out_z) {
    float yaw_rad = yaw_deg * (float)M_PI / 180.0f;
    float cos_y = cosf(yaw_rad);
    float sin_y = sinf(yaw_rad);

    *out_x = x * cos_y + z * sin_y;
    *out_y = y;
    *out_z = -x * sin_y + z * cos_y;
}

// Hilfsfunktion: Rotation um die X-Achse (Pitch)
static void rotate_pitch(float x, float y, float z, float pitch_deg, float *out_x, float *out_y, float *out_z) {
    float pitch_rad = pitch_deg * (float)M_PI / 180.0f;
    float cos_p = cosf(pitch_rad);
    float sin_p = sinf(pitch_rad);

    *out_x = x;
    *out_y = y * cos_p - z * sin_p;
    *out_z = y * sin_p + z * cos_p;
}

// Hilfsfunktion: Rotation um die Z-Achse (Roll)
void rotate_roll(float x, float y, float z, float roll_deg, float *out_x, float *out_y, float *out_z) {
    float roll_rad = roll_deg * (float)M_PI / 180.0f;
    float cos_r = cosf(roll_rad);
    float sin_r = sinf(roll_rad);

    *out_x = x * cos_r - y * sin_r;
    *out_y = x * sin_r + y * cos_r;
    *out_z = z;
}

// Hauptfunktion: Rotation in der Reihenfolge Yaw → Pitch → Roll
void rotatePoint(float x, float y, float z, 
                        float yaw_deg, float pitch_deg, float roll_deg,
                        float *out_x, float *out_y, float *out_z) {
    float tx, ty, tz;

    // Schritt 1: Yaw (Rotation um Y-Achse)
    rotate_yaw(x, y, z, yaw_deg, &tx, &ty, &tz);

    // Schritt 2: Pitch (Rotation um X-Achse)
    rotate_pitch(tx, ty, tz, pitch_deg, &tx, &ty, &tz);

    // Schritt 3: Roll (Rotation um Z-Achse)
    rotate_roll(tx, ty, tz, roll_deg, out_x, out_y, out_z);
}

// Hauptfunktion: Rotation in der Reihenfolge Roll → Pitch → Yaw
void rotatePoint_reverse(float x, float y, float z, 
                        float yaw_deg, float pitch_deg, float roll_deg,
                        float *out_x, float *out_y, float *out_z) {
    float tx, ty, tz;

    // Schritt 1: Yaw (Rotation um Y-Achse)
    rotate_roll(x, y, z, roll_deg, &tx, &ty, &tz);

    // Schritt 2: Pitch (Rotation um X-Achse)
    rotate_pitch(tx, ty, tz, pitch_deg, &tx, &ty, &tz);

    // Schritt 3: Roll (Rotation um Z-Achse)
    rotate_yaw(tx, ty, tz, yaw_deg, out_x, out_y, out_z);
}

void transformTriangles(float x, float y, float z, float yaw, float pitch, float roll)
{
    // Winkel in Radiant umwandeln (falls sie in Grad übergeben werden)
    // Falls du bereits Radiant erwartest, entferne die folgenden drei Zeilen:

    for (size_t i = 0; i < MAX_TRIANGLES; ++i)
    {
        const struct triangle *src = &triangles[i];
        struct triangle *dst = &triangles_transformed[i];
        dst->free = src->free;
        if (src->free == false) {

            // Farben bleiben unverändert
            dst->c1 = src->c1;
            dst->c2 = src->c2;
            dst->c3 = src->c3;

            // Vertex 1 transformieren
            rotatePoint(src->x1-x, src->y1-y, src->z1-z, yaw, pitch, roll,
                        &dst->x1, &dst->y1, &dst->z1);

            // Vertex 2 transformieren
            rotatePoint(src->x2-x, src->y2-y, src->z2-z, yaw, pitch, roll,
                        &dst->x2, &dst->y2, &dst->z2);

            // Vertex 3 transformieren
            rotatePoint(src->x3-x, src->y3-y, src->z3-z, yaw, pitch, roll,
                        &dst->x3, &dst->y3, &dst->z3);
        }
    }
}

uint32_t RGB(int r,int g, int b) {
    return (r&255) | (g&255) << 8 | (b&255) << 16;
}

uint32_t RGB888_to_RGBA8888(uint32_t rgb)
{
    return (rgb&255) << 24 | (rgb&(255<<8)) << 8 | (rgb&(255<<16)) >> 8;
}

void drawTriangle(uint32_t* buffer, int bufferWidth, int bufferHeight,
                  float x1, float y1, float z1, uint32_t c1,
                  float x2, float y2, float z2, uint32_t c2,
                  float x3, float y3, float z3, uint32_t c3) {
    const float d = 1.0f; // Entfernung der Projektionsebene

    // Hinter der Kamera liegende Dreiecke ignorieren
    if (z1 <= 0 || z2 <= 0 || z3 <= 0) return;
    if (z1 > cameraDistance || z2 > cameraDistance || z3 > cameraDistance ) return;

    // Perspektivprojektion: (x,y,z) → (x*d/z, y*d/z)
    float u1 = (x1 * d) / z1;
    float v1 = (y1 * d) / z1;
    float u2 = (x2 * d) / z2;
    float v2 = (y2 * d) / z2;
    float u3 = (x3 * d) / z3;
    float v3 = (y3 * d) / z3;

    // In Pixelkoordinaten umwandeln
    float scaleX = bufferHeight / 2.0f;
    float scaleY = bufferHeight / 2.0f;
    float offsetX = bufferWidth / 2.0f;
    float offsetY = bufferHeight / 2.0f;

    int px1 = (int)(u1 * scaleX + offsetX);
    int py1 = (int)(-v1 * scaleY + offsetY);
    int px2 = (int)(u2 * scaleX + offsetX);
    int py2 = (int)(-v2 * scaleY + offsetY);
    int px3 = (int)(u3 * scaleX + offsetX);
    int py3 = (int)(-v3 * scaleY + offsetY);
    
    if (px1 < -1000 || px1 > bufferWidth+1000 || py1 < -1000 || py1 > bufferHeight+1000 ||
        px2 < -1000 || px2 > bufferWidth+1000 || py2 < -1000 || py2 > bufferHeight+1000 ||
        px3 < -1000 || px3 > bufferWidth+1000 || py3 < -1000 || py3 > bufferHeight+1000) return;

    // Bounding Box
    int minX = (int)fmaxf(0, fminf(px1, fminf(px2, px3)));
    int maxX = (int)fminf(bufferWidth - 1, fmaxf(px1, fmaxf(px2, px3)));
    int minY = (int)fmaxf(0, fminf(py1, fminf(py2, py3)));
    int maxY = (int)fminf(bufferHeight - 1, fmaxf(py1, fmaxf(py2, py3)));

    // Werte für perspektiv-korrekte Interpolation vorbereiten
    float invZ1 = 1.0f / z1;
    float invZ2 = 1.0f / z2;
    float invZ3 = 1.0f / z3;

    // Baryzentrische Denominator (einmalig)
    float denom = (py2 - py3) * (px1 - px3) + (px3 - px2) * (py1 - py3);
    if (fabsf(denom) < 1e-6f) return; // Degeneriertes Dreieck

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            // Baryzentrische Gewichte
            float w1 = ((py2 - py3) * (x - px3) + (px3 - px2) * (y - py3)) / denom;
            float w2 = ((py3 - py1) * (x - px3) + (px1 - px3) * (y - py3)) / denom;
            float w3 = 1.0f - w1 - w2;

            if (w1 >= 0.0f && w2 >= 0.0f && w3 >= 0.0f) {
                // Perspektiv-korrekte Tiefeninterpolation: 1/z interpolieren
                float invZ = w1 * invZ1 + w2 * invZ2 + w3 * invZ3;
                float zInterpolated = 1.0f / invZ;

                // Depth-Test (näher = größerer z-Wert, da z positiv in Blickrichtung)
                int idx = y * bufferWidth + x;
                if (zInterpolated >= zBuffer[idx]) // weiter weg oder gleich → überspringen
                    continue;
                zBuffer[idx] = zInterpolated; // neuen Wert speichern

                // Farbe baryzentrisch interpolieren (lineare Interpolation im Screen-Space ist für Gouraud ausreichend)
                // Separate Kanäle extrahieren
                uint32_t r = (uint32_t)((( c1        & 0xFF) * w1) +
                                        (( c2        & 0xFF) * w2) +
                                        (( c3        & 0xFF) * w3));
                uint32_t g = (uint32_t)((((c1 >> 8)  & 0xFF) * w1) +
                                        (((c2 >> 8)  & 0xFF) * w2) +
                                        (((c3 >> 8)  & 0xFF) * w3));
                uint32_t b = (uint32_t)((((c1 >> 16) & 0xFF) * w1) +
                                        (((c2 >> 16) & 0xFF) * w2) +
                                        (((c3 >> 16) & 0xFF) * w3));
                uint32_t a = 255; // Alpha bleibt voll deckend

                uint32_t color = (r&0xFF)<<24 | (g&0xFF)<<16 | (b&0xFF)<<8 | (a&0xFF);

                buffer[y * bufferWidth + x] = color;
            }
        }
    }
}

void drawLine(uint32_t* buffer, int bufferWidth, int bufferHeight,
                     int x0, int y0, float z0, uint32_t c0,
                     int x1, int y1, float z1, uint32_t c1)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int steps = dx > dy ? dx : dy;
    float invSteps = steps > 0 ? 1.0f / steps : 0.0f;

    float z = z0;
    float dz = (z1 - z0) * invSteps;

    float r = (float)(c0 & 0xFF);
    float g = (float)((c0 >> 8) & 0xFF);
    float b = (float)((c0 >> 16) & 0xFF);

    float dr = ((float)(c1 & 0xFF)        - r) * invSteps;
    float dg = ((float)((c1 >> 8) & 0xFF) - g) * invSteps;
    float db = ((float)((c1 >> 16)& 0xFF) - b) * invSteps;

    for (;;) {
        if (x0 >= 0 && x0 < bufferWidth && y0 >= 0 && y0 < bufferHeight) {
            int idx = y0 * bufferWidth + x0;

            if (z < zBuffer[idx]) {
                zBuffer[idx] = z;
                buffer[idx] =
                    ((uint32_t)r & 0xFF) << 24 |
                    ((uint32_t)g & 0xFF) << 16 |
                    ((uint32_t)b & 0xFF) << 8  |
                    0xFF;
            }
        }

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = err << 1;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }

        z += dz;
        r += dr; g += dg; b += db;
    }
}

void drawTriangleWireframe(uint32_t* buffer, int bufferWidth, int bufferHeight,
                           float x1, float y1, float z1, uint32_t c1,
                           float x2, float y2, float z2, uint32_t c2,
                           float x3, float y3, float z3, uint32_t c3)
{
    const float d = 1.0f;

    // Hinter der Kamera → weg
    if (z1 <= 0 || z2 <= 0 || z3 <= 0) return;
    if (z1 > cameraDistance || z2 > cameraDistance || z3 > cameraDistance ) return;

    // Projektion
    float u1 = (x1 * d) / z1;
    float v1 = (y1 * d) / z1;
    float u2 = (x2 * d) / z2;
    float v2 = (y2 * d) / z2;
    float u3 = (x3 * d) / z3;
    float v3 = (y3 * d) / z3;

    float scale = bufferHeight * 0.5f;
    float ox = bufferWidth * 0.5f;
    float oy = bufferHeight * 0.5f;

    int px1 = (int)( u1 * scale + ox);
    int py1 = (int)(-v1 * scale + oy);
    int px2 = (int)( u2 * scale + ox);
    int py2 = (int)(-v2 * scale + oy);
    int px3 = (int)( u3 * scale + ox);
    int py3 = (int)(-v3 * scale + oy);
    
    if (px1 < -1000 || px1 > bufferWidth+1000 || py1 < -1000 || py1 > bufferHeight+1000 ||
        px2 < -1000 || px2 > bufferWidth+1000 || py2 < -1000 || py2 > bufferHeight+1000 ||
        px3 < -1000 || px3 > bufferWidth+1000 || py3 < -1000 || py3 > bufferHeight+1000) return;

    // Drei Kanten zeichnen
    drawLine(buffer, bufferWidth, bufferHeight,
             px1, py1, z1, c1, px2, py2, z2, c2);

    drawLine(buffer, bufferWidth, bufferHeight,
             px2, py2, z2, c2, px3, py3, z3, c3);

    drawLine(buffer, bufferWidth, bufferHeight,
             px3, py3, z3, c3, px1, py1, z1, c1);
}

void drawTrianglePoints(uint32_t* buffer, int bufferWidth, int bufferHeight,
                  float x1, float y1, float z1, uint32_t c1,
                  float x2, float y2, float z2, uint32_t c2,
                  float x3, float y3, float z3, uint32_t c3)
{
    const float d = 1.0f;

    // Hinter der Kamera → weg
    if (z1 <= 0 || z2 <= 0 || z3 <= 0) return;
    if (z1 > cameraDistance || z2 > cameraDistance || z3 > cameraDistance ) return;

    // Perspektivprojektion
    float u1 = (x1 * d) / z1;
    float v1 = (y1 * d) / z1;
    float u2 = (x2 * d) / z2;
    float v2 = (y2 * d) / z2;
    float u3 = (x3 * d) / z3;
    float v3 = (y3 * d) / z3;

    // Screen-Mapping
    float scale = bufferHeight / 2.0f;
    float offsetX = bufferWidth  / 2.0f;
    float offsetY = bufferHeight / 2.0f;

    int px[3] = {
        (int)( u1 * scale + offsetX ),
        (int)( u2 * scale + offsetX ),
        (int)( u3 * scale + offsetX )
    };

    int py[3] = {
        (int)(-v1 * scale + offsetY ),
        (int)(-v2 * scale + offsetY ),
        (int)(-v3 * scale + offsetY )
    };

    float pz[3] = { z1, z2, z3 };
    uint32_t col[3] = { c1, c2, c3 };

    // Drei Eckpunkte zeichnen
    for (int i = 0; i < 3; ++i)
    {
        int x = px[i];
        int y = py[i];
        int c = col[i];

        // Screen-Clipping
        if (x < 0 || x >= bufferWidth ||
            y < 0 || y >= bufferHeight)
            continue;

        int idx = y * bufferWidth + x;

        // Depth-Test (kleiner z = näher)
        if (pz[i] >= zBuffer[idx])
            continue;

        zBuffer[idx] = pz[i];
        
        uint32_t r = (uint32_t)( c        & 0xFF);
        uint32_t g = (uint32_t)((c >> 8)  & 0xFF);
        uint32_t b = (uint32_t)((c >> 16) & 0xFF);
        uint32_t a = 255; // Alpha bleibt voll deckend

        buffer[idx] = (r&0xFF)<<24 | (g&0xFF)<<16 | (b&0xFF)<<8 | (a&0xFF);
    }
}

void clearBuffer(uint32_t* buffer, int bufferWidth, int bufferHeight, uint32_t color) {
    for (int y = 0; y < bufferHeight; y++) {
        for (int x = 0; x < bufferWidth; x++) {
            buffer[y * bufferWidth + x] = color;
            zBuffer[y * bufferWidth + x] = 1e9f;
        }
    }
}

void drawTriangle_(uint32_t* buffer, int bufferWidth, int bufferHeight, int mode,
                  float x1, float y1, float z1, uint32_t c1,
                  float x2, float y2, float z2, uint32_t c2,
                  float x3, float y3, float z3, uint32_t c3) {
    if (mode == RENDER_MODE_NORMAL) {
        drawTriangle(buffer,bufferWidth,bufferHeight,x1,y1,z1,c1,x2,y2,z2,c2,x3,y3,z3,c3);
    } else if (mode == RENDER_MODE_WIREFRAME) {
        drawTriangleWireframe(buffer,bufferWidth,bufferHeight,x1,y1,z1,c1,x2,y2,z2,c2,x3,y3,z3,c3);
    } else if (mode == RENDER_MODE_POINTS) {
        drawTrianglePoints(buffer,bufferWidth,bufferHeight,x1,y1,z1,c1,x2,y2,z2,c2,x3,y3,z3,c3);
    }
}

void drawTriangles(uint32_t* framebuffer, float x, float y, float z, float yaw, float pitch, float roll, int mode) {
    transformTriangles(x,y,z,yaw,pitch,roll);
    for (size_t i = 0; i < MAX_TRIANGLES; ++i)
    {
        if (triangles_transformed[i].free == false) {
            drawTriangle_(framebuffer,WINDOW_WIDTH,WINDOW_HEIGHT,mode,
                        triangles_transformed[i].x1,triangles_transformed[i].y1,triangles_transformed[i].z1,triangles_transformed[i].c1,
                        triangles_transformed[i].x2,triangles_transformed[i].y2,triangles_transformed[i].z2,triangles_transformed[i].c2,
                        triangles_transformed[i].x3,triangles_transformed[i].y3,triangles_transformed[i].z3,triangles_transformed[i].c3);
        }
    }
}

