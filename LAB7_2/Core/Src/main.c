/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "fatfs_sd.h"
#include "string.h"
#include "stdio.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

SPI_HandleTypeDef hspi1;
FATFS fs;
FATFS *pfs;
FIL fil;
FRESULT fres;
DWORD fre_clust;
uint32_t totalSpace, freeSpace;
char buffer[100];

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

//Variables de manejo de Menu

uint8_t flag_menu = 1; // Bandera de control de menu
uint8_t RX1_buffer = 0; //Variable de recepción UART

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

void transmit_uart(char *string);
void enviar_gojo(void);
void enviar_asta(void);
void enviar_senku(void);

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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  HAL_UART_Receive_IT(&huart2, &RX1_buffer, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


	  while(flag_menu){

		  transmit_uart("------Desplegar Imagenes-----");
		  transmit_uart("a. Ajolote");
		  transmit_uart("b. Cerdo");
		  transmit_uart("c. Esqueleto");

		  flag_menu = 0; //Apagar menu
	  }

	  if(flag_menu == 0){

		  switch(RX1_buffer){

		  case'a':
			  transmit_uart("----Ajolote----");

			  enviar_gojo();
			  RX1_buffer = 0;
			  flag_menu = 1;
		  break;

		  case 'b':
			  transmit_uart("----Cerdo----");

			  enviar_asta();
			  RX1_buffer = 0;
			  flag_menu = 1;
		  break;

		  case 'c':
			  transmit_uart("----Esqueleto----");

			  enviar_senku();
			  RX1_buffer = 0;
			  flag_menu = 1;
		  break;

		  }
	  }

    /* USER CODE END WHILE */

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  HAL_GPIO_WritePin(SD_SS_GPIO_Port, SD_SS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SD_SS_Pin */
  GPIO_InitStruct.Pin = SD_SS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(SD_SS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void transmit_uart(char *string)
{
	char spaceBuffer[1]="\n";
    HAL_UART_Transmit(&huart2, (uint8_t *)string, strlen(string), HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart2, spaceBuffer, 1, HAL_MAX_DELAY);
}

void transmit_uart_n(char *string)
{
	char spaceBuffer[1]="\n";
    HAL_UART_Transmit(&huart2, (uint8_t *)string, strlen(string), HAL_MAX_DELAY);
    //HAL_UART_Transmit(&huart2, spaceBuffer, 1, HAL_MAX_DELAY);
}



void enviar_gojo(void){

	//enviar a Gojo

	//MONTAR SD
	f_mount(&fs, "/", 0);
	if(fres == FR_OK){
		transmit_uart("SD montada correctamente");
	} else if (fres == !FR_OK){
		transmit_uart("ERROR: SD no pudo ser montada");
	}


	// ABRIR ARCHIVO
	f_open(&fil, "ajolote.txt",FA_READ);
	if(fres == FR_OK){
		transmit_uart("Archivo abierto correctamente");
	} else if (fres == !FR_OK){
		transmit_uart("ERROR: Archivo no pudo abrirse correctamente");
	}

	//LEER ARCHIVO
	transmit_uart("Leyendo");
	while(f_gets(buffer, sizeof(buffer), &fil)){
		char mRd[100];
		sprintf(mRd, "%s", buffer);
		transmit_uart_n(mRd);
	}
	transmit_uart("Lectura temrinada");

	//CERRAR EL ARCHIVO

	fres = f_close(&fil);
	if(fres == FR_OK){
		transmit_uart("Archivo cerrado correctamente");
	} else if (fres == !FR_OK){
		transmit_uart("ERROR: Archivo no pudo cerrarse correctamente");
	}

	//DESMONTAR
	f_mount(NULL, "", 1);
	if(fres == FR_OK){
		transmit_uart("SD desmontada correctamente");
	} else if (fres == !FR_OK){
		transmit_uart("ERROR: SD no pudo desmontarse correctamente");
	}

}
void enviar_asta(void){

	//Enviar a Asta
	//MONTAR SD
	f_mount(&fs, "/", 0);
	if(fres == FR_OK){
		transmit_uart("SD montada correctamente");
	} else if (fres == !FR_OK){
		transmit_uart("ERROR: SD no pudo ser montada");
	}


	// ABRIR ARCHIVO
	f_open(&fil, "cerdo.txt", FA_READ);
	if(fres == FR_OK){
		transmit_uart("Archivo abierto correctamente");
	} else if (fres == !FR_OK){
		transmit_uart("ERROR: Archivo no pudo abrirse correctamente");
	}

	//LEER ARCHIVO

	while(f_gets(buffer, sizeof(buffer), &fil)){
		char mRd[100];
		sprintf(mRd, "%s", buffer);
		transmit_uart_n(mRd);
	}

	//CERRAR EL ARCHIVO

	fres = f_close(&fil);
	if(fres == FR_OK){
		transmit_uart("Archivo cerrado correctamente");
	} else if (fres == !FR_OK){
		transmit_uart("ERROR: Archivo no pudo cerrarse correctamente");
	}

	//DESMONTAR
	f_mount(NULL, "", 1);
	if(fres == FR_OK){
		transmit_uart("SD desmontada correctamente");
	} else if (fres == !FR_OK){
		transmit_uart("ERROR: SD no pudo desmontarse correctamente");
	}



}
void enviar_senku(void){

	//Enviar a Senku

	//MONTAR SD
	f_mount(&fs, "/", 0);
	if(fres == FR_OK){
		transmit_uart("SD montada correctamente");
	} else if (fres == !FR_OK){
		transmit_uart("ERROR: SD no pudo ser montada");
	}


	// ABRIR ARCHIVO
	f_open(&fil, "esqueleto.txt", FA_READ);
	if(fres == FR_OK){
		transmit_uart("Archivo abierto correctamente");
	} else if (fres == !FR_OK){
		transmit_uart("ERROR: Archivo no pudo abrirse correctamente");
	}

	//LEER ARCHIVO

	while(f_gets(buffer, sizeof(buffer), &fil)){
		char mRd[100];
		sprintf(mRd, "%s", buffer);
		transmit_uart_n(mRd);
	}

	//CERRAR EL ARCHIVO

	fres = f_close(&fil);
	if(fres == FR_OK){
		transmit_uart("Archivo cerrado correctamente");
	} else if (fres == !FR_OK){
		transmit_uart("ERROR: Archivo no pudo cerrarse correctamente");
	}

	//DESMONTAR
	f_mount(NULL, "", 1);
	if(fres == FR_OK){
		transmit_uart("SD desmontada correctamente");
	} else if (fres == !FR_OK){
		transmit_uart("ERROR: SD no pudo desmontarse correctamente");
	}

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */

  HAL_UART_Receive_IT(&huart2, &RX1_buffer, 1);
  //transmit_uart("Interrupcion :D");

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
