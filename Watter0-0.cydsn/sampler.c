#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sampler.h"

void sampler(void* arg){
    sampleEffekt_t* parameter = (sampleEffekt_t*)arg;
    
    while(1){
        
        xSemaphoreTake(*(parameter->mutex),0);
        (*(parameter->power))++;
        (*(parameter->cadance))+= 2;
        xSemaphoreGive(*(parameter->mutex));
        vTaskDelay(50);   
    }
}