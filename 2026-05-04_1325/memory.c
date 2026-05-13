int initMemory(void) {
    objects = malloc(sizeof(struct object) * MAX_OBJECTS);
    triangles = malloc(sizeof(struct triangle) * MAX_TRIANGLES);
    triangles_transformed = malloc(sizeof(struct triangle) * MAX_TRIANGLES);
    path = malloc(sizeof(struct pathPoint) * MAX_PATHS * MAX_PATH_POINTS);
    zBuffer = malloc(sizeof(float) * 128 * 1048576);
    pathLeangth = malloc(sizeof(int) * MAX_PATHS);
    pathLineID = malloc(sizeof(int) * MAX_PATHS);

    if (!objects || !triangles || !triangles_transformed ||
        !path || !zBuffer || !pathLeangth || !pathLineID)
    {
        fprintf(stderr, "initMemory(): allocation failed\n");
        return 0;   // oder exit(), je nach Philosophie
    }

    return 1;
}

void clearMemory(void) {
    for (size_t i = 0; i < 128 * 1048576; ++i)
        zBuffer[i] = 1.0f;

    for (int i = 0; i < MAX_PATHS; ++i)
        pathLeangth[i] = 0;
}

void freeMemory(void) {
    free(objects);
    free(triangles);
    free(triangles_transformed);
    free(path);
    free(zBuffer);
    free(pathLeangth);
    free(pathLineID);

    objects = NULL;
    triangles = NULL;
    triangles_transformed = NULL;
    path = NULL;
    zBuffer = NULL;
    pathLeangth = NULL;
    pathLineID = NULL;
}
