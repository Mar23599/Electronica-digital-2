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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

uint8_t flag_game_start = 0; // En cero, el juego no ha empezado. En 1, el juego empezo.
uint8_t flag_winner = 0; // Si esta en 1: gano PL1; Si esta en 2: gano PL2.


//Variables de jugador 1

uint8_t PL1_counter = 0;
uint8_t PL1_round_counter = 0;
uint16_t PL1_counter_exit = 0;

//Variables de jugador 2

uint8_t PL2_counter = 0;
uint8_t PL2_round_counter = 0;
uint16_t PL2_counter_exit = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//Función para enceder leds del jugador 1
void player1_out(uint8_t contador){

	HAL_GPIO_WritePin(GPIOA,PL0_Pin|PL1_Pin|PL2_Pin , GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, PL3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, PL4_Pin|PL5_Pin|PL6_Pin|PL7_Pin, GPIO_PIN_RESET);

	//Imprimir el contador
	if (contador & 0b00000001){

		HAL_GPIO_WritePin(GPIOA, PL0_Pin, GPIO_PIN_SET);
	}

	if (contador & 0b00000010){
		HAL_GPIO_WritePin(GPIOA, PL1_Pin, GPIO_PIN_SET);
	}

	if (contador & 0b00000100){
		HAL_GPIO_WritePin(GPIOA, PL2_Pin, GPIO_PIN_SET);
	}

	if (contador & 0b00001000){
		HAL_GPIO_WritePin(GPIOB, PL3_Pin, GPIO_PIN_SET);
	}

	if (contador & 0b00010000){
		HAL_GPIO_WritePin(GPIOC, PL4_Pin, GPIO_PIN_SET);
	}

	if (contador & 0b00100000){
		HAL_GPIO_WritePin(GPIOC, PL5_Pin, GPIO_PIN_SET);
	}

	if (contador & 0b01000000){
		HAL_GPIO_WritePin(GPIOC, PL6_Pin, GPIO_PIN_SET);
	}

	if (contador & 0b10000000){
		HAL_GPIO_WritePin(GPIOC, PL7_Pin, GPIO_PIN_SET);
	}

}

void player2_out (uint8_t contador){

	// Actualizar los pines cada vez que se llame a la función
	HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, LD0_Pin|LD1_Pin|LD3_Pin|LD4_Pin|LD5_Pin|LD6_Pin|LD7_Pin, GPIO_PIN_RESET);

	//Imprimir el contador
	if (contador & 0b00000001){

		HAL_GPIO_WritePin(GPIOB, LD0_Pin, GPIO_PIN_SET);
	}

	if (contador & 0b00000010){
		HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_SET);
	}

	if (contador & 0b00000100){
		HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_SET);
	}

	if (contador & 0b00001000){
		HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);
	}

	if (contador & 0b00010000){
		HAL_GPIO_WritePin(GPIOB, LD4_Pin, GPIO_PIN_SET);
	}

	if (contador & 0b00100000){
		HAL_GPIO_WritePin(GPIOB, LD5_Pin, GPIO_PIN_SET);
	}

	if (contador & 0b01000000){
		HAL_GPIO_WritePin(GPIOB, LD6_Pin, GPIO_PIN_SET);
	}

	if (contador & 0b10000000){
		HAL_GPIO_WritePin(GPIOB, LD7_Pin, GPIO_PIN_SET);
	}

}

void semaforo_out(void){

	//Mostrar secuencia de semáforo
	HAL_Delay(3000);
	HAL_GPIO_WritePin(GPIOC, RED_Pin, GPIO_PIN_SET);
	HAL_Delay(2000);
	HAL_GPIO_WritePin(GPIOC, YELL_Pin, GPIO_PIN_SET);
	HAL_Delay(2000);
	HAL_GPIO_WritePin(GPIOC, GREEN_Pin, GPIO_PIN_SET);

}

uint8_t Potencia_base_2(uint8_t exponente){

	uint8_t resultado = 1;

	if (exponente == 0){
		return 1;
	}

	for (uint8_t i = 0; 1 < exponente; i++){

		resultado *= 2;
	}

	return resultado;

}

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
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  HAL_GPIO_WritePin(GPIOC, PL5_Pin|PL4_Pin|PL6_Pin|PL7_Pin
                          |GREEN_Pin|YELL_Pin|RED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, PL1_Pin|PL0_Pin|PL2_Pin|LD2_Pin
                          |LD2A10_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, PL3_Pin|LD3_Pin|LD4_Pin|LD7_Pin
                          |LD6_Pin|LD5_Pin|LD0_Pin|LD1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PL5_Pin PL4_Pin PL6_Pin PL7_Pin
                           GREEN_Pin YELL_Pin RED_Pin */
  GPIO_InitStruct.Pin = PL5_Pin|PL4_Pin|PL6_Pin|PL7_Pin
                          |GREEN_Pin|YELL_Pin|RED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PL1_Pin PL0_Pin PL2_Pin LD2_Pin
                           LD2A10_Pin */
  GPIO_InitStruct.Pin = PL1_Pin|PL0_Pin|PL2_Pin|LD2_Pin
                          |LD2A10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PL3_Pin LD3_Pin LD4_Pin LD7_Pin
                           LD6_Pin LD5_Pin LD0_Pin LD1_Pin */
  GPIO_InitStruct.Pin = PL3_Pin|LD3_Pin|LD4_Pin|LD7_Pin
                          |LD6_Pin|LD5_Pin|LD0_Pin|LD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PL1_WIN_Pin PL2_WIN_Pin */
  GPIO_InitStruct.Pin = PL1_WIN_Pin|PL2_WIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB_1_Pin PB_2_Pin PB_start_Pin */
  GPIO_InitStruct.Pin = PB_1_Pin|PB_2_Pin|PB_start_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

	void HAL_GPIO_EXIT_Callback(uint16_t pin){

	if(flag_game_start == 0){

		if(HAL_GPIO_ReadPin(GPIOC, PB_start_Pin) == GPIO_PIN_RESET){

			//Si se presiona el boton de start
			flag_game_start = 1; //Bandera indica que el juego empezó

			//Iniciar secuencia de start
			semaforo_out(); //El programa se mantiene en aqui aproximadamente 7 segundos
		}
	}

	if(flag_game_start == 1){

		//Con la bandera en 1, el juego puede empezar.

		if(HAL_GPIO_ReadPin(GPIOC, PB_1_Pin) == GPIO_PIN_RESET){

			//Si se presiona el boton de Player1
			PL1_counter++; // Cada que se presione el botón, incrementar contador

			if (PL1_counter > 8){

				PL1_counter = 0; // Cada 8 veces, resetear el conteo
				PL1_round_counter++; //e incrementar el numero de vueltas

				if(PL1_round_counter == 3){
					//Cuando se alcanzen las 3 vueltas
					flag_winner = 1; // Gano el jugador 1
				}
			}

			//Actualizar contador de salida
			PL1_counter_exit = Potencia_base_2(PL1_counter); // Aplicar el contador octal

		}

		if(HAL_GPIO_ReadPin(GPIOC, PB_2_Pin) == GPIO_PIN_RESET){

			//Si se presiona el boton de Player2
			PL2_counter++; // Cada que se presione el botón, incrementar contador

			if (PL2_counter > 8){

				PL2_counter = 0; // Cada 8 veces, resetear el conteo
				PL2_round_counter++; //e incrementar el numero de vueltas

				if(PL2_round_counter == 3){
					//Cuando se alcanzen las 3 vueltas
					flag_winner = 2;
				}

			}

			//Actualizar contador de salida

			PL2_counter_exit = Potencia_base_2(PL2_counter);

		}
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
