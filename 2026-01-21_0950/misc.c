uint64_t startTime;

void timerInit(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    startTime = (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

uint64_t get_ms_since_start(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000) - startTime;
}

void init_random() {
    srand((unsigned int)time(NULL));
}

// 1. Zufällige Ganzzahl von 0 bis max (inklusive)
uint32_t random_max(uint32_t max) {
    // Vermeide Overflow und Bias bei großen Zahlen
    uint32_t threshold = UINT32_MAX - (UINT32_MAX % (max + 1));
    uint32_t r;
    do {
        r = rand() * (RAND_MAX / UINT32_MAX) + rand(); // besserer 32-Bit-Wert
    } while (r >= threshold);
    return r % (max + 1);
}

// 2. Zufällige Ganzzahl zwischen min und max (inklusive)
uint32_t random_min_max(uint32_t min, uint32_t max) {
    if (min > max) {
        uint32_t temp = min;
        min = max;
        max = temp;
    }
    return min + random_max(max - min);
}

// 3. Zufällige Gleitkommazahl von 0.0 bis max (inklusive)
float rand_max(float max) {
    return ((float)rand() / (float)RAND_MAX) * max;
}

// 4. Zufällige Gleitkommazahl zwischen min und max (inklusive)
float rand_min_max(float min, float max) {
    if (min > max) {
        float temp = min;
        min = max;
        max = temp;
    }
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}
