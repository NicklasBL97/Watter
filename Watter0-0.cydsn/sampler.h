#pragma once
#include "project.h"
#include "semphr.h"

typedef struct sampleEffekt_t {
    int16* power;
    int16* cadance;
    SemaphoreHandle_t* mutex;
}sampleEffekt_t;

void sampler(void* arg);