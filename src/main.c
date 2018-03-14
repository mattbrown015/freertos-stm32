#include "../inc/stm32l0xx_hal_msp.h"

#include <FreeRTOS.h>
#include <task.h>

#include <stm32l0xx_hal.h>
#include <stm32l0538_discovery.h>

#include <stdlib.h>

static UART_HandleTypeDef uartHandle
    = {
        .Instance = USARTx
        , .Init.BaudRate = 9600
        , .Init.WordLength = UART_WORDLENGTH_8B
        , .Init.StopBits = UART_STOPBITS_1
        , .Init.Parity = UART_PARITY_NONE
        , .Init.HwFlowCtl = UART_HWCONTROL_NONE
        , .Init.Mode = UART_MODE_TX_RX
    };

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

static void uartReceiveTask()
{
    for (;;)
    {
        static uint8_t uartReceiveBuffer;

        HAL_UART_Receive(&uartHandle, &uartReceiveBuffer, sizeof(uartReceiveBuffer), HAL_MAX_DELAY);
        HAL_UART_Transmit(&uartHandle, &uartReceiveBuffer, sizeof(uartReceiveBuffer), HAL_MAX_DELAY);
    }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 32000000
  *            HCLK(Hz)                       = 32000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_MUL                        = 4
  *            PLL_DIV                        = 2
  *            Flash Latency(WS)              = 1
  *            Main regulator output voltage  = Scale1 mode
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet. */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Enable HSI Oscillator and activate PLL with HSI as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
    RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
    RCC_OscInitStruct.HSICalibrationValue = 0x10;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
}

static HAL_StatusTypeDef transmit_greeting(void)
{
    /*const*/uint8_t greeting[]
        = { 'f', 'r', 'e', 'e', 'r', 't', 'o', 's', '-', 's', 't', 'm', '3', '2', '\r', '\n' };
    return HAL_UART_Transmit(&uartHandle, greeting, sizeof(greeting), HAL_MAX_DELAY);
}

int main()
{
    HAL_Init();

    /* Configure the system clock to 32 MHz */
    SystemClock_Config();

    BSP_LED_Init(LED3);
    BSP_LED_Init(LED4);

    const HAL_StatusTypeDef init_result = HAL_UART_Init(&uartHandle);
    if (init_result != HAL_OK)
        return EXIT_FAILURE;

    if (transmit_greeting() != HAL_OK)
        return EXIT_FAILURE;

    const BaseType_t result0 = xTaskCreate(toggleLed3Task, "toggleLed3Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    if (result0 != pdPASS)
        return EXIT_FAILURE;

    const BaseType_t result1 = xTaskCreate(toggleLed4Task, "toggleLed4Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    if (result1 != pdPASS)
        return EXIT_FAILURE;

    const BaseType_t result2 = xTaskCreate(uartReceiveTask, "uartReceiveTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    if (result2 != pdPASS)
        return EXIT_FAILURE;

    vTaskStartScheduler();

    return EXIT_FAILURE;
}
