/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 * ?��경�?
 * 기능?���??
 * 1. 버튼?�� ?��르면 비콘 모드�?? 켜진?��. - ?��?�� HAL_GPIO_EXTI_Callback?��?�� ?��?��?�� ?�� ?��?��.
 * 2. beacon 모드?�� UID 모드?��.
 * 3. namespaceID = "liveinSIOR" = 0x6c 69 76 65 69 6e 53 49 4f 52
 * 4. instanceID = 0x01 - eddystonebeacon.h ?��?��?��?�� ?��?�� �???��
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "dfsdm.h"
#include "i2c.h"
#include "octospi.h"
#include "spi.h"
#include "usb_otg.h"
#include "gpio.h"
#include "app_bluenrg_ms.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "lsm6dsl.h"
#include "lsm6dsl_reg.h"
#include "b_l4s5i_iot01a_bus.h"
#include "b_l4s5i_iot01a_errno.h"

#include "ai_platform.h"
#include "network.h"
#include "network_data.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
LSM6DSL_Object_t MotionSensor;
volatile uint32_t dataRdyIntReceived;

ai_handle network;
float aiInData[AI_NETWORK_IN_1_SIZE];
float aiOutData[AI_NETWORK_OUT_1_SIZE];
uint8_t activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];
const char* activities[AI_NETWORK_OUT_1_SIZE] = {
        "Gesture", "Stationary"
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void MEMS_Init(void);
void BLE_UID_ON(uint32_t Delay);
static void AI_Init(ai_handle w_addr, ai_handle act_addr);
static void AI_Run(float *pIn, float *pOut);
static uint32_t argmax(const float * values, uint32_t len);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_DFSDM1_Init();
    MX_I2C1_Init();
    MX_OCTOSPI1_Init();
    MX_SPI1_Init();
    MX_USB_OTG_FS_USB_Init();
    MX_CRC_Init();
    AI_Init(ai_network_data_weights_get(), activations);
    MX_BlueNRG_MS_Init();
    /* USER CODE BEGIN 2 */
    MEMS_Init();

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    uint32_t write_index = 0;
    while (1) {
        //transmit zyro sensor coordinates by UART.
        if (dataRdyIntReceived != 0) {
            dataRdyIntReceived = 0;
            LSM6DSL_Axes_t acc_axes;
            LSM6DSL_ACC_GetAxes(&MotionSensor, &acc_axes);
            //printf("%5d, %5d, %5d\r\n", (int) acc_axes.x, (int) acc_axes.y, (int) acc_axes.z);

            aiInData[write_index + 0] = (float) acc_axes.x;
            aiInData[write_index + 1] = (float) acc_axes.y;
            aiInData[write_index + 2] = (float) acc_axes.z;

            write_index += 3;
            if (write_index == AI_NETWORK_IN_1_SIZE)
            {
                write_index = 0;

                printf("Running inference\r\n");
                AI_Run(aiInData, aiOutData);

                for (uint32_t i = 0; i < AI_NETWORK_OUT_1_SIZE; i++)
                {
                    printf("%8.6f ", aiOutData[i]);
                }
                uint32_t class = argmax(aiOutData, AI_NETWORK_OUT_1_SIZE);
                printf(": %d - %s\r\n", (int)class, activities[class]);
                if (class == 0) {
                    BLE_UID_ON(1000);
                }
            }
        }
        /* USER CODE END WHILE */

        //MX_BlueNRG_MS_Process();
        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Configure the main internal regulator output voltage
     */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST) != HAL_OK)
    {
        Error_Handler();
    }
    /** Configure LSE Drive Capability
     */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 60;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
            |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1
            |RCC_PERIPHCLK_I2C2|RCC_PERIPHCLK_DFSDM1
            |RCC_PERIPHCLK_USB|RCC_PERIPHCLK_ADC
            |RCC_PERIPHCLK_OSPI;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
    PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
    PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
    PeriphClkInit.Dfsdm1ClockSelection = RCC_DFSDM1CLKSOURCE_PCLK;
    PeriphClkInit.OspiClockSelection = RCC_OSPICLKSOURCE_SYSCLK;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
    PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
    PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
    PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
    PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK|RCC_PLLSAI1_ADC1CLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
    /** Enable MSI Auto calibration
     */
    HAL_RCCEx_EnableMSIPLLMode();
}

/* USER CODE BEGIN 4 */
void BLE_UID_ON(uint32_t Delay) {
    printf("BLE ON\r\n");
    EddystoneUID_Start();
    HAL_Delay(Delay);
    HCI_TL_SPI_Reset();
    MX_BlueNRG_MS_Init();
    printf("BLE OFF\r\n");
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == BUTTON_EXTI13_Pin) {
        BLE_UID_ON(5000);
    }
    if (GPIO_Pin == LSM6DSL_INT1_EXTI11_Pin) {
        dataRdyIntReceived++;
    }
}

static void MEMS_Init(void) {
    LSM6DSL_IO_t io_ctx;
    uint8_t id;
    LSM6DSL_AxesRaw_t axes;

    /* Link I2C functions to the LSM6DSL driver */
    io_ctx.BusType = LSM6DSL_I2C_BUS;
    io_ctx.Address = LSM6DSL_I2C_ADD_L;
    io_ctx.Init = BSP_I2C2_Init;
    io_ctx.DeInit = BSP_I2C2_DeInit;
    io_ctx.ReadReg = BSP_I2C2_ReadReg;
    io_ctx.WriteReg = BSP_I2C2_WriteReg;
    io_ctx.GetTick = BSP_GetTick;
    LSM6DSL_RegisterBusIO(&MotionSensor, &io_ctx);

    /* Read the LSM6DSL WHO_AM_I register */
    LSM6DSL_ReadID(&MotionSensor, &id);
    if (id != LSM6DSL_ID) {
        Error_Handler();
    }

    /* Initialize the LSM6DSL sensor */
    LSM6DSL_Init(&MotionSensor);

    /* Configure the LSM6DSL accelerometer (ODR, scale and interrupt) */
    LSM6DSL_ACC_SetOutputDataRate(&MotionSensor, 26.0f); /* 26 Hz */
    LSM6DSL_ACC_SetFullScale(&MotionSensor, 4); /* [-4000mg; +4000mg] */
    LSM6DSL_ACC_Set_INT1_DRDY(&MotionSensor, ENABLE); /* Enable DRDY */
    LSM6DSL_ACC_GetAxesRaw(&MotionSensor, &axes); /* Clear DRDY */

    /* Start the LSM6DSL accelerometer */
    LSM6DSL_ACC_Enable(&MotionSensor);
}

static void AI_Init(ai_handle w_addr, ai_handle act_addr)
{
    ai_error err;

    err = ai_network_create(&network, AI_NETWORK_DATA_CONFIG);
    if (err.type != AI_ERROR_NONE)
    {
        printf("ai_network_create error - type=%d code=%d\r\n", err.type, err.code);
        Error_Handler();
    }
    const ai_network_params params =
    {
            AI_NETWORK_DATA_WEIGHTS(w_addr),
            AI_NETWORK_DATA_ACTIVATIONS(act_addr)
    };

    if(!ai_network_init(network, &params))
    {
        err = ai_network_get_error(network);
        printf("ai_network_init error - type=%d code=%d\r\n", err.type, err.code);
        Error_Handler();
    }
}

static void AI_Run(float *pIn, float *pOut)
{
    ai_i32 batch;
    ai_error err;

    ai_buffer ai_input[AI_NETWORK_IN_NUM] = AI_NETWORK_IN;
    ai_buffer ai_output[AI_NETWORK_OUT_NUM] = AI_NETWORK_OUT;

    ai_input[0].n_batches = 1;
    ai_input[0].data = AI_HANDLE_PTR(pIn);
    ai_output[0].n_batches = 1;
    ai_output[0].data = AI_HANDLE_PTR(pOut);

    batch = ai_network_run(network, ai_input, ai_output);
    if (batch != 1)
    {
        err = ai_network_get_error(network);
        printf("AI ai_network_run error - type%d code=%d\r\n", err.type, err.code);
        Error_Handler();
    }
}

static uint32_t argmax(const float *values, uint32_t len)
{
    float max_value = values[0];
    uint32_t max_index = 0;
    for (uint32_t i = 1; i < len ; i++)
    {
        if (values[i] > max_value)
        {
            max_value = values[i];
            max_index = i;
        }
    }
    return max_index;
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
