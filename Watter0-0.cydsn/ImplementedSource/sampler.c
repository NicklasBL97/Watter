#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sampler.h"
#include "bleHandler.h"

sampleEffekt_t samples;

void task_sampler(void* arg){
    sampleEffekt_t* parameter = (sampleEffekt_t*)arg;
    
    while(1){
        
        xSemaphoreTake(*(parameter->mutex),0);
        //(*(parameter->power))++;
        (*(parameter->cadance))+= 2;
        xSemaphoreGive(*(parameter->mutex));
        vTaskDelay(parameter->delay);   
    }
}

void sampler_init(){
    
    
    samples.power = &sendEffectInfo.power;
    samples.cadance = &sendEffectInfo.cadance;
    samples.delay = 50;
    powerMutex = xSemaphoreCreateMutex();
    samples.mutex = &powerMutex;
}