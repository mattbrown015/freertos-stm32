#include <FreeRTOS.h>
#include <task.h>

#include "stm32l0xx_hal.h"

extern void xPortSysTickHandler(void);

void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();

    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
}
