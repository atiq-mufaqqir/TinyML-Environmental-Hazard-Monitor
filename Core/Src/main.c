/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bme68x_necessary_functions.h"  // Main header from the library
#include <stdio.h>

#include "scd4x_i2c.h"
#include "sensirion_i2c_hal.h"
#include <stdio.h>
#include "sps30_i2c.h"

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
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */


//Data structure for BME688 sensor from the driver.
struct bme68x_data bme_data;


//Data structure for SCD41 sensor
typedef struct {
    uint16_t co2;           // CO₂ in ppm
    int32_t    temperature;   // °C
    int32_t    humidity;      // %RH
} scd41_data_t;

scd41_data_t scd41_data;



//Data structure for SPS30 sensor
typedef struct {
    float mc_1p0;   // Mass concentration PM1.0 [µg/m³]
    float mc_2p5;
    float mc_4p0;
    float mc_10p0;

    float nc_0p5;   // Number concentration [#/cm³]
    float nc_1p0;
    float nc_2p5;
    float nc_4p0;
    float nc_10p0;

    float typical_particle_size;  // µm
} sps30_data_t;

sps30_data_t sps30_data;






uint32_t last_frame_time = 0;
uint32_t reading_num = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */




int ei_add_sensor_reading(
    float co2,
    float temp,
    float humidity,
    float pressure,
    float gas_resistance,
    float pm1_0,
    float pm2_5,
    float pm4_0,
    float pm10_0,
    float pnc_0_5,
    float pnc_1_0,
    float pnc_2_5,
    float pnc_4_0,
    float pnc_10_0,
    float tps
);

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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */



// I2C SCANNER FUNCTION

  void I2C_Scan(I2C_HandleTypeDef *hi2c)
  {
      printf("Scanning I2C bus...\r\n");

      for (uint8_t i = 1; i < 127; i++)
      {
          if (HAL_I2C_IsDeviceReady(hi2c, (i << 1), 2, 10) == HAL_OK)
          {
              printf("Device found at 0x%02X\r\n", i);
          }
      }

      printf("Scan complete\r\n");
  }



  I2C_Scan(&hi2c1);




    if (bme68x_start(&bme_data, &hi2c1) != 0) {
        printf("BME688 initialization failed!\r\n");
        while(1);
    }
    printf("BME688 initialized successfully!\r\n");

    sensirion_i2c_hal_init();

        scd4x_init(0x62);                                 // Initialise sensor

        scd4x_stop_periodic_measurement();                // Safe start

        int16_t err = scd4x_start_periodic_measurement(); // Start continuous measurement
        if (err != 0) {
            printf("SCD41 initialisation failed! Error = %d\r\n", err);
            while(1);
        }

        printf("SCD41 initialised successfully!\r\n");
        printf("Waiting for first reading (~5-10 seconds)...\r\n\r\n");



        sps30_init(SPS30_I2C_ADDR_69);

        int16_t ret = sps30_wake_up_sequence();
            if (ret != 0) {
                printf("SPS30 wake-up failed: %d\r\n", ret);
            }



            ret = sps30_start_measurement(SPS30_OUTPUT_FORMAT_OUTPUT_FORMAT_FLOAT);
                if (ret != 0) {
                    printf("SPS30 start measurement failed! Error: %d\r\n", ret);
                } else {
                    printf("SPS30 initialised successfully (Float mode)\r\n");
                }


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


        HAL_Delay(15000);
          printf(",co2,temp,humidity,pressure,gas_resistance,pm1_0,pm2_5,pm4_0,pm10_0,pnc_0_5,pnc_1_0,pnc_2_5,pnc_4_0,pnc_10_0,tps\r\n");



  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


	  uint32_t now = HAL_GetTick();


	      if (sps30_read_measurement_values_float(
	              &sps30_data.mc_1p0, &sps30_data.mc_2p5,
	              &sps30_data.mc_4p0, &sps30_data.mc_10p0,
	              &sps30_data.nc_0p5, &sps30_data.nc_1p0,
	              &sps30_data.nc_2p5, &sps30_data.nc_4p0,
	              &sps30_data.nc_10p0, &sps30_data.typical_particle_size) != 0)
	      {

	      }


	      bme68x_single_measure(&bme_data);

	        ei_add_sensor_reading(
	            (float)scd41_data.co2,
	            bme_data.temperature,
	            bme_data.humidity,
	            bme_data.pressure,
	            bme_data.gas_resistance,
	            sps30_data.mc_1p0,
	            sps30_data.mc_2p5,
	            sps30_data.mc_4p0,
	            sps30_data.mc_10p0,
	            sps30_data.nc_0p5,
	            sps30_data.nc_1p0,
	            sps30_data.nc_2p5,
	            sps30_data.nc_4p0,
	            sps30_data.nc_10p0,
	            sps30_data.typical_particle_size
	        );


	      if (now - last_frame_time >= 5000)   // 5 second MASTER FRAME
	      {
	          last_frame_time = now;

	          scd4x_get_data_ready_status(false);
	          scd4x_read_measurement(
	              &scd41_data.co2,
	              &scd41_data.temperature,
	              &scd41_data.humidity
	          );


	          uint32_t ms = now;
	          uint32_t seconds = ms / 1000;
	          uint32_t minutes = seconds / 60;
	          uint32_t hours   = minutes / 60;

	          seconds %= 60;
	          minutes %= 60;
	          hours %= 24;


		       // printf("%02lu:%02lu:%02lu, %lu, %4u, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\r\n",
				        printf(",%4u, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\r\n",

		                       scd41_data.co2,
		                       bme_data.temperature, bme_data.humidity, bme_data.pressure,bme_data.gas_resistance,
		                          sps30_data.mc_1p0, sps30_data.mc_2p5,
		                          sps30_data.mc_4p0, sps30_data.mc_10p0,
		                          sps30_data.nc_0p5, sps30_data.nc_1p0,
		                          sps30_data.nc_2p5, sps30_data.nc_4p0,
		                          sps30_data.nc_10p0, sps30_data.typical_particle_size);

//   TIME | # | CO2 | TEMP | HUMIDITY |PRESSURE | GAS RESISTANCE | PM1.0 | PM2.5| PM4.0 | PM10.0| PNC 0.5 | PNC 1.0 | PNC 2.5 | PNC 4.0 |PNC 10.0| TPS





	      }











	/*




	  int16_t ret = sps30_read_measurement_values_float(
	                          &sps30_data.mc_1p0, &sps30_data.mc_2p5,
	                          &sps30_data.mc_4p0, &sps30_data.mc_10p0,
	                          &sps30_data.nc_0p5, &sps30_data.nc_1p0,
	                          &sps30_data.nc_2p5, &sps30_data.nc_4p0,
	                          &sps30_data.nc_10p0, &sps30_data.typical_particle_size);

	          if (ret != 0) {
	              printf("SPS30 read error: %d\r\n", ret);
	          }





	    bme68x_single_measure(&bme_data);


	    scd4x_get_data_ready_status(false);
	    scd4x_read_measurement(&scd41_data.co2, &scd41_data.temperature, &scd41_data.humidity);





	        uint32_t ms = HAL_GetTick();

	        uint32_t seconds = ms / 1000;
	        uint32_t minutes = seconds / 60;
	        uint32_t hours   = minutes / 60;

	        seconds = seconds % 60;
	        minutes = minutes % 60;
	        hours   = hours % 24;

	        printf(" %02lu:%02lu:%02lu | #%lu | CO2= %4uppm | TEMP= %.2f°C | HUMIDITY= %.2f%% |PRESSURE = %.2f | GAS RESISTANCE = %.2f | PM1.0= %.2f | PM2.5= %.2f | PM4.0= %.2f | PM10.0= %.2f | PNC 0.5= %.2f | PNC 1.0= %.2f | PNC 2.5= %.2f | PNC 4.0= %.2f |PNC 10.0= %.2f  | TPS= %.2f \r\n",

	        	       hours, minutes, seconds,
	        		reading_num++,
	                       scd41_data.co2,
	                       bme_data.temperature, bme_data.humidity, bme_data.pressure,bme_data.gas_resistance,
	                          sps30_data.mc_1p0, sps30_data.mc_2p5,
	                          sps30_data.mc_4p0, sps30_data.mc_10p0,
	                          sps30_data.nc_0p5, sps30_data.nc_1p0,
	                          sps30_data.nc_2p5, sps30_data.nc_4p0,
	                          sps30_data.nc_10p0, sps30_data.typical_particle_size);


*/


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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10D19CE4;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */




  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */


  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
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
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
