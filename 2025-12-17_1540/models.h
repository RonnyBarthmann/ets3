#define true 1
#define false 0

int createTerrain(float x,float y,float z, float width, float leangth, float tileSize, int color);
int createLine();
void addToLine(int line_id, float x1,float y1,float z1, float x2, float y2, float z2, int color);
void createPath(int path_id, float x, float y, float z);
void freePath(int path_id);
void addToPath(int path_id, float x, float y, float z, int color);
void lookAt(float targetX, float targetY, float targetZ);
void moveCameraOnPath(int path_id, int direction, float position);
int load_path_from_file(int path_id, const char* filename, int color);
