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

