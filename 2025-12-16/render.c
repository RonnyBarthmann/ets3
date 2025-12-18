#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "render.h"

struct triangle triangles[MAX_TRIANGLES];
struct triangle triangles_transformed[MAX_TRIANGLES];
float zBuffer[128*1048576];

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

void transformTriangles(float x, float y, float z, float yaw, float pitch, float roll)
{
    // Winkel in Radiant umwandeln (falls sie in Grad übergeben werden)
    // Falls du bereits Radiant erwartest, entferne die folgenden drei Zeilen:
    yaw  = yaw  * (M_PI / 180.0f);
    pitch  = pitch  * (M_PI / 180.0f);
    roll   = roll   * (M_PI / 180.0f);

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

    // Farben in RGBA8888 konvertieren (angenommen RGB888_to_RGBA8888 fügt Alpha=255 hinzu)

    // Hinter der Kamera liegende Dreiecke ignorieren
    if (z1 <= 0 || z2 <= 0 || z3 <= 0) return;

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

void clearBuffer(uint32_t* buffer, int bufferWidth, int bufferHeight, uint32_t color) {
    for (int y = 0; y < bufferHeight; y++) {
        for (int x = 0; x < bufferWidth; x++) {
            buffer[y * bufferWidth + x] = color;
            zBuffer[y * bufferWidth + x] = 1e9f;
        }
    }
}

void drawTriangles(uint32_t* framebuffer, float x, float y, float z, float yaw, float pitch, float roll) {
    transformTriangles(x,y,z,yaw,pitch,roll);
    for (size_t i = 0; i < MAX_TRIANGLES; ++i)
    {
        if (triangles_transformed[i].free == false) {
            drawTriangle(framebuffer,WINDOW_WIDTH,WINDOW_HEIGHT,
                        triangles_transformed[i].x1,triangles_transformed[i].y1,triangles_transformed[i].z1,triangles_transformed[i].c1,
                        triangles_transformed[i].x2,triangles_transformed[i].y2,triangles_transformed[i].z2,triangles_transformed[i].c2,
                        triangles_transformed[i].x3,triangles_transformed[i].y3,triangles_transformed[i].z3,triangles_transformed[i].c3);
        }
    }
}
