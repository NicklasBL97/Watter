#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "limits.h"

#include "PowerMode.h"
#include "bleHandler.h"

cy_stc_syspm_callback_params_t callbackParams = {
	    /*.base       =*/ NULL,
	    /*.context    =*/ NULL
	};
    
    cy_stc_syspm_callback_t PwmDeepSleepCb = {TCPWM_DeepSleepCallback,  /* Callback function */
                                          CY_SYSPM_DEEPSLEEP,       /* Callback type */
                                          CY_SYSPM_SKIP_CHECK_READY |
                                          CY_SYSPM_SKIP_CHECK_FAIL, /* Skip mode */
										  &callbackParams,       	/* Callback params */
                                          NULL, NULL};				/* For internal usage */


cy_en_syspm_status_t TCPWM_DeepSleepCallback(
    cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode)
{
    cy_en_syspm_status_t retVal = CY_SYSPM_FAIL;

	switch (mode)
	{
		case CY_SYSPM_BEFORE_TRANSITION:
			/* Before going to sleep mode, turn off stuff */
			retVal = CY_SYSPM_SUCCESS;
			break;

		case CY_SYSPM_AFTER_TRANSITION:
			/* Re-enable stuff */
			retVal = CY_SYSPM_SUCCESS;
			break;

		default:
			/* Don't do anything in the other modes */
			retVal = CY_SYSPM_SUCCESS;
			break;
	}

    return retVal;
}

void task_gotoDeepSleep(void* arg){
    
    while(1){
        if(connState == NOT_CONNECTED){
            vTaskDelay(60000);
            if(connState == NOT_CONNECTED)
                Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
        }
            
    }
}
