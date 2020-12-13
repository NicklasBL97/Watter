#pragma once
#include "project.h"

void task_gotoDeepSleep(void* arg);
cy_en_syspm_status_t TCPWM_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode);