#pragma once
#include "project.h"
#include "semphr.h"

typedef struct sampleEffekt_t {
    int16* power;
    int16* cadance;
    uint16 delay;
    SemaphoreHandle_t* mutex;
}sampleEffekt_t;

extern sampleEffekt_t samples;


/***************************************************************************
Function to sample power output and cadance.

This function periodically samples from sensors and stores information about to power output and cadance.
The function is implemented as a FreeTROS task, and should be added to scheduler using xTaskCreate().
 @param arg void pointer to the datastructure where sampled values is placed.
 @return void.
****************************************************************************/
void task_sampler(void* arg);

void sampler_init();