#include "../inc/stm32l0xx_hal_msp.h"

#include <FreeRTOS.h>
#include <task.h>

#include <stm32l0xx_hal.h>
#include <stm32l0538_discovery.h>

#include <stdlib.h>

static void toggleLed3Task()
{
    for (;;)
    {
        BSP_LED_Toggle(LED3);

        const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
        vTaskDelay(xDelay);
    }
}

static void toggleLed4Task()
{
    for (;;)
    {
        BSP_LED_Toggle(LED4);

        const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
        vTaskDelay(xDelay);
    }
}

int main()
{
    HAL_Init();

    BSP_LED_Init(LED3);
    BSP_LED_Init(LED4);

    const BaseType_t result0 = xTaskCreate(toggleLed3Task, "toggleLed3Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    if (result0 != pdPASS)
        return EXIT_FAILURE;

    const BaseType_t result1 = xTaskCreate(toggleLed4Task, "toggleLed4Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    if (result1 != pdPASS)
        return EXIT_FAILURE;

    vTaskStartScheduler();

    return EXIT_FAILURE;
}
