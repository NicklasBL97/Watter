/**
 * @file sampler.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-12-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once
#include "project.h"
#include "semphr.h"

/**
 * @brief 
 * 
 */
typedef struct sampleEffekt_t {
    int16* power;
    int16* cadance;
    uint16 delay;
    SemaphoreHandle_t* mutex;
}sampleEffekt_t;

/**
 * @brief 
 * 
 */
extern sampleEffekt_t samples;

/**
 * @brief 
 * This function periodically samples from sensors and stores information about to power output and cadance.
 * The function is implemented as a FreeTROS task, and should be added to scheduler using xTaskCreate().
 * 
 * @param arg void pointer to the datastructure where sampled values is placed.
 */
void task_sampler(void* arg);

/**
 * @brief 
 * 
 */
void sampler_init();