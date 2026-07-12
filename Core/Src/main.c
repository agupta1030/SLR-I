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
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define GYRO_SPI_READ  0x80  // 1000 0000 (Sets MSB to 1)
#define GYRO_SPI_WRITE 0x7F  // 0111 1111 (Clears MSB to 0)
#define CRSF_BUF_SIZE 64
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t gyro_data_ready = 0;
volatile uint32_t now_cycles = 0;
uint32_t last_cycles = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
	  uint8_t tx_address;
	  uint8_t rx_response;
	  uint8_t crsf_rx_buf[CRSF_BUF_SIZE];
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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_TIM6_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(3000);
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  //Receiver USART2
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, crsf_rx_buf, CRSF_BUF_SIZE);

  HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET);
  HAL_Delay(50);

  tx_address = 0x75 | GYRO_SPI_READ;

  HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_RESET);
  HAL_Delay(5); // Hold it low for a tiny moment before clocks start

  HAL_SPI_Transmit(&hspi1, &tx_address, 1, HAL_MAX_DELAY);

  HAL_SPI_Receive(&hspi1, &rx_response, 1, HAL_MAX_DELAY);

  HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET);
  HAL_Delay(5);

  HAL_Delay(2000);
  static char diagnostic_buf[256];
    int msg_len = snprintf(diagnostic_buf, sizeof(diagnostic_buf),
                           "\r\n========================================\r\n"
                           "Radiolink F722 IMU Diagnostic Verification\r\n"
                           "Target ID Expected: 0x47\r\n"
                           "Sensor ID Received: 0x%02X\r\n"
                           "STATUS: %s\r\n"
                           "========================================\r\n\r\n",
                           rx_response,
                           (rx_response == 0x47) ? "SUCCESS! SPI communication is valid." : "ERROR! Communication fault.");

    CDC_Transmit_FS((uint8_t*)diagnostic_buf, msg_len);
    HAL_Delay(100);

  uint8_t init_buf[2];
  init_buf[0] = 0x4E & GYRO_SPI_WRITE;
  init_buf[1] = 0x0F;

  HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_RESET);

  for(volatile int i = 0; i < 50; i++);

  HAL_SPI_Transmit(&hspi1, init_buf, 2, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET);
  HAL_Delay(50);

  // INT_CONFIG register (0x14) - Sets up INT1 pin formatting
	// 0x03 sets INT1 to push-pull, active low (matching your EXTI falling edge)
	init_buf[0] = 0x14 & GYRO_SPI_WRITE;
	init_buf[1] = 0x03;

	HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_RESET);
	for(volatile int i = 0; i < 50; i++);
	HAL_SPI_Transmit(&hspi1, init_buf, 2, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET);
	HAL_Delay(50);

	// INT_SOURCE0 register (0x65) - Routes data ready to INT1 pin
	// 0x08 enables UI data ready interrupt routing
	init_buf[0] = 0x65 & GYRO_SPI_WRITE;
	init_buf[1] = 0x08;

	HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_RESET);
	for(volatile int i = 0; i < 50; i++);
	HAL_SPI_Transmit(&hspi1, init_buf, 2, HAL_MAX_DELAY);

  HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET);
  HAL_Delay(50);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  float g_roll = 0;
  float g_pitch = 0;
  float g_yaw = 0;

  float a_roll = 0;
  float a_pitch = 0;

  float roll_estimate = 0;
  float pitch_estimate = 0;

  float uncertainty = 0;
  float gyro_drift_rate = 0.01f;
  float accel_noise_level = 0.05f;

  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  if(gyro_data_ready){
	  gyro_data_ready = 0;

	  //finding the change in time between gyro measurements
	  uint32_t dt_cycles = now_cycles - last_cycles;
	  last_cycles = now_cycles;
	  float dt_s = dt_cycles/216000000.0f;

	  // Create an 8-byte buffer to accommodate the address + 6 data bytes safely
	  uint8_t spi_tx_buf[15] = {0};
	  uint8_t spi_rx_buf[15] = {0};

	  // Set the first byte to the target register address with the read bit flag
	  spi_tx_buf[0] = 0x1D | GYRO_SPI_READ;

	  // Toggle CS Low to select the IMU
	  HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_RESET);
	  for(volatile int i = 0; i < 50; i++); // Safe electrical settling delay

	  // Transmit and receive simultaneously over the 7-byte window
	  HAL_SPI_TransmitReceive(&hspi1, spi_tx_buf, spi_rx_buf, 15, HAL_MAX_DELAY);

	  // Deselect IMU immediately
	  HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET);

	  /* * Note: spi_rx_buf[0] contains dummy data returned while we transmitted the address.
	   * The actual gyro data lives in elements [1] through [6].
	   */

	  int16_t temp_raw = (int16_t)((spi_rx_buf[1] << 8) | spi_rx_buf[2]);

	  int16_t ax_raw = (int16_t)((spi_rx_buf[3] << 8) | spi_rx_buf[4]);
	  int16_t ay_raw = (int16_t)((spi_rx_buf[5] << 8) | spi_rx_buf[6]);
	  int16_t az_raw = (int16_t)((spi_rx_buf[7] << 8) | spi_rx_buf[8]);

	  int16_t gx_raw = (int16_t)((spi_rx_buf[9] << 8) | spi_rx_buf[10]);
	  int16_t gy_raw = (int16_t)((spi_rx_buf[11] << 8) | spi_rx_buf[12]);
	  int16_t gz_raw = (int16_t)((spi_rx_buf[13] << 8) | spi_rx_buf[14]);

	  // ICM-42688-P default sensitivity scale factor is 16.4 LSB/dps
	  float gx_dps = gx_raw / 16.4f;
	  float gy_dps = gy_raw / 16.4f;
	  float gz_dps = gz_raw / 16.4f;

	  g_roll += gx_dps * dt_s;
	  g_pitch += gy_dps * dt_s;
	  g_yaw += gz_dps * dt_s;


	  float ax_g = ax_raw / 2048.0f;
	  float ay_g = ay_raw / 2048.0f;
	  float az_g = az_raw / 2048.0f;



	  roll_estimate += gx_dps * dt_s;
	  pitch_estimate += gy_dps * dt_s;

	  uncertainty += gyro_drift_rate * dt_s;


	  a_roll = atan2f(ay_g, az_g) * (180.0f/M_PI);
	  a_pitch = atan2f(ax_g, az_g) * (180.0f/M_PI);

	  float a_weight = uncertainty/(uncertainty + accel_noise_level);

	  roll_estimate += a_weight * (a_roll - roll_estimate);
	  pitch_estimate += a_weight * (a_pitch - pitch_estimate);

	  uncertainty *= (1.0f - a_weight);


	  static char buf[128];
	  int n = snprintf(buf, sizeof(buf), "Roll: %.4f | Pitch: %.4f\r\n", roll_estimate, pitch_estimate);
	  CDC_Transmit_FS((uint8_t*)buf, n);
  	  }
  	  //HAL_Delay(1000);
//	  static char test_msg[] = "USB is alive with peripherals active!\r\n";
//	  CDC_Transmit_FS((uint8_t*)test_msg, sizeof(test_msg) - 1);
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GYRO_DRDY_Pin) // Safe check against your custom label
    {
    	now_cycles = DWT->CYCCNT;
        gyro_data_ready = 1;
    }
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
