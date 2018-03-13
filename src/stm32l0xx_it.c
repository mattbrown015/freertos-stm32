#include "stm32l0xx_hal.h"

extern void xPortSysTickHandler(void);

void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();

    xPortSysTickHandler();
}
