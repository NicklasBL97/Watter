

#include <stdio.h>
#include "project.h"
#include "adcsample.h"



void initADCsample()
{
    ADC_1_Start();
    ADC_1_StartConvert();  
}

int getADCsample()
{
    
    int sample = 0;
 
   // if (ADC_1_IsEndConversion(ADC_1_WAIT_FOR_RESULT))
    //{
        uint16_t result = ADC_1_GetResult16(0);  
        int16 spaending = ADC_1_CountsTo_mVolts(0, result);
        sample = spaending;
    //}
    
    
    return sample;   
}




