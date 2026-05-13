#define MAX_CHUNKS 256
#define CHUNK_SIZE 4

struct node {
    int node;
};

struct chunk {
    int free;
    int object;
    int x, y, z;
    struct node node[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];
};

struct chunk chunks[MAX_CHUNKS];

int getChunkID(int x, int y, int z) {
    for (int i=0;i<MAX_CHUNKS;i++) {
        if (chunks[i].free == 0) {
            if (chunks[i].x == x) {
                if (chunks[i].y == y) {
                    if (chunks[i].z == z) {
                        return i;
                    }
                }
            }
        }
    }
    return -1;
};

int newChunk(int x, int y, int z) {
    int chunk_id = getChunkID(x, y, z);
    if (chunk_id != -1) return chunk_id;
    
    for (int i = 0; i < MAX_CHUNKS; i++) {
        if (chunks[i].free == true) {  // freier Platz gefunden
            chunk_id = i;
            break;
        }
    }
    
    // Wenn kein freier Platz mehr vorhanden ist → abbrechen
    if (chunk_id == -1) {
        printf("Maximum an Chunks (%d) erreicht.\n", MAX_CHUNKS);
        return -1;
    }
    
    chunks[chunk_id].free = false;
    chunks[chunk_id].object = create_object();
    if (chunks[chunk_id].object == -1) {
        printf("Failed to get Object for Chunk.\n");
        chunks[chunk_id].free = true;
        return -1;
    }
    chunks[chunk_id].x = x;
    chunks[chunk_id].y = y;
    chunks[chunk_id].z = z;
    for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE; i++) {
        chunks[chunk_id].node[i].node = 0;
    }
};

void generateChunk(int chunk_id) {
    printf("Generate Chunk %d at (%d,%d,%d)\n", chunk_id, chunks[chunk_id].x, chunks[chunk_id].y, chunks[chunk_id].z);
    
};

void updateChunkMesh(int chunk_id) {
    printf("Update Chunk Mesh of Chunk %d at (%d,%d,%d)\n", chunk_id, chunks[chunk_id].x, chunks[chunk_id].y, chunks[chunk_id].z);
};

void emergeChunk(int x, int y, int z) {
    printf("Emerge Chunk (%d,%d,%d)\n", x, y, z);
    if (getChunkID(x, y, z) == -1) {
        int chunk_id = newChunk(x,y,z);
        if (chunk_id == -1) return;
        generateChunk(chunk_id);
        updateChunkMesh(chunk_id);
    };
};

void updateChunks() {
};

void initChunks() {
    for (int i=0;i<MAX_CHUNKS;i++) chunks[i].free = 1;
    emergeChunk(0,0,0);
};
