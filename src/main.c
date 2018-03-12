#include <stm32l0xx_hal.h>
#include <stm32l0538_discovery.h>

int main()
{
    HAL_Init();

    BSP_LED_Init(LED3);
    BSP_LED_Init(LED4);

    BSP_LED_Toggle(LED3);
    BSP_LED_Toggle(LED4);

    return 0;
}
