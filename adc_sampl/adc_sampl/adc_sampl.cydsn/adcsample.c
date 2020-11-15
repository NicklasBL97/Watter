

#include <stdio.h>
#include "project.h"



void initSample()
{
    ADC_1_Start();
    ADC_1_StartConvert();  
}

int getSample()
{
    
    int sample = 0;
 
   // if (ADC_1_IsEndConversion(ADC_1_WAIT_FOR_RESULT))
    //{
        uint16_t result = ADC_1_GetResult16();  
        int16 spaending = ADC_1_CountsTo_mVolts(result);
        sample = spaending;
    //}
    
    
    return sample;   
}




