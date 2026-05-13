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
