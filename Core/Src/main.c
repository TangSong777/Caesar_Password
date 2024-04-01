/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "KEY.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
	OUTPUT = 0,
	EZINPUT = 1,
	INPUT = 2
} PUT_STATE; // 两种模式（通过按键切换）

typedef enum
{
	mode1 = 0,
	mode2,
	mode3
} LED_STATE;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
PUT_STATE PutState = OUTPUT; // 默认为输出信号
LED_STATE LedState = mode1;
uint8_t Key_flag = 0; // 按键标志量
uint8_t Morse_len = 0;
uint8_t str_len = 0;
uint8_t Space_num = 0;
uint8_t Morse[50];
uint8_t str[200];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Delay_break(uint16_t ms)
{
	for (uint8_t temp = 0; temp < ms / 10; temp++)
	{
		if (Key_flag)
			break;
		HAL_Delay(10);
	}
}

int judge(uint8_t data[], const char a[], int len)
{
	for (int i = 0; i < len; i++)
	{
		if (data[i] == a[i])
			continue;
		else
			return 0;
	}
	return 1;
}

void transform(uint8_t Morse[], uint8_t str[], uint8_t Morse_len, uint8_t i)
{
	switch (Morse_len)
	{
	case 1:
	{
		if (judge(Morse, "a", 1))
			str[i] = 'e';
		if (judge(Morse, "b", 1))
			str[i] = 't';
		break;
	}
	case 2:
	{
		if (judge(Morse, "ab", 2))
			str[i] = 'a';
		if (judge(Morse, "aa", 2))
			str[i] = 'i';
		if (judge(Morse, "bb", 2))
			str[i] = 'm';
		if (judge(Morse, "ba", 2))
			str[i] = 'n';
		break;
	}
	case 3:
	{
		if (judge(Morse, "baa", 3))
			str[i] = 'd';
		if (judge(Morse, "bba", 3))
			str[i] = 'g';
		if (judge(Morse, "bab", 3))
			str[i] = 'k';
		if (judge(Morse, "bbb", 3))
			str[i] = 'o';
		if (judge(Morse, "aba", 3))
			str[i] = 'r';
		if (judge(Morse, "aaa", 3))
			str[i] = 's';
		if (judge(Morse, "aab", 3))
			str[i] = 'u';
		if (judge(Morse, "abb", 3))
			str[i] = 'w';
		break;
	}
	case 4:
	{
		if (judge(Morse, "baaa", 4))
			str[i] = 'b';
		if (judge(Morse, "baba", 4))
			str[i] = 'c';
		if (judge(Morse, "aaba", 4))
			str[i] = 'f';
		if (judge(Morse, "aaaa", 4))
			str[i] = 'h';
		if (judge(Morse, "abbb", 4))
			str[i] = 'j';
		if (judge(Morse, "abaa", 4))
			str[i] = 'l';
		if (judge(Morse, "abba", 4))
			str[i] = 'p';
		if (judge(Morse, "bbab", 4))
			str[i] = 'q';
		if (judge(Morse, "aaab", 4))
			str[i] = 'v';
		if (judge(Morse, "baab", 4))
			str[i] = 'x';
		if (judge(Morse, "babb", 4))
			str[i] = 'y';
		if (judge(Morse, "bbaa", 4))
			str[i] = 'z';
		break;
	}
	}
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
	MX_DMA_Init();
	MX_USART2_UART_Init();
	MX_TIM10_Init();
	/* USER CODE BEGIN 2 */
	__HAL_TIM_CLEAR_IT(&htim10, TIM_IT_UPDATE);
	HAL_TIM_Base_Start_IT(&htim10);
	Key_Init(KEY0, KEY_GPIO_Port, KEY_Pin, PULL_UP);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		if (PutState == OUTPUT) // 当模式为‘发送信号模式时’
		{
			if (Key_flag == 1)
			{
				if (LedState < mode3)
					LedState++;
				else
					LedState = 0;
			}
			else if (Key_flag == 3)
				PutState = EZINPUT; // 长按切换为‘接收信号’模式

			switch (LedState)
			{
			case mode1:
			{
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
				Delay_break(1000);
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
				Delay_break(1000);
				break;
			}
			case mode2:
			{
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
				Delay_break(2000);
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
				Delay_break(2000);
				break;
			}
			case mode3:
			{
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
				Delay_break(3000);
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
				Delay_break(3000);
				break;
			}
			}
		}
		if (PutState == EZINPUT)
		{
		}
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
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 100;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM10) // 0.01s
	{
		if (PutState == OUTPUT || PutState == EZINPUT)
			Key_pressscan(KEY0, &Key_flag);
		if (PutState == EZINPUT)
		{
			/*
			接收光电二极管信号，当接收到亮信号时Bright_time++;暗信号时Dark_time++;
			if(Bright_time == Dark_time && Bright_time == 100)
			{
				printf("发起进攻");
				Bright_time = 0;
				Dark_time = 0;
			}
			*/
		}
		if (PutState == INPUT)
		{
			/*
			uint8_t Bright_time = 0,Dark_time = 0,start_input = 0;
			接收光电二极管信号，当接收到亮信号时Bright_time++;暗信号时Dark_time++;
			if(亮信号)
				{
					if(start_input == 0)start_input = 1;//发送第一个光信号的时候视为开始发送
					Bright_time++;
					Dark_time = 0;
				}
			if(start_input == 1)
				{
				if(暗信号)
					{
						Dark_time++；
						if(Dark_time == 1)
							{
								如果光信号长度不为0
			const uint8_t i=0;
			if(Bright_time==1)
				Morse[i++]='a';Bright_time=0;
			else if(Bright_time==3)
				Morse[i++]='b';Bright_time=0;
								检测光信号长度 将其储存到凯撒代码数组中 清零光信号长度
								如果为0 不进行处理
							}
						else if(Dark_time == 3)
							{
								字符停顿
								处理凯撒代码字符串
				transform(Morse,str,Morse_len,str_len++);
								将凯撒代码转化成字符储存到字符串数组 清零储存凯撒代码的数组
								如果凯撒代码数组已被清零 则不进行处理
							}
						else if(Dark_time == 7)
							{
								单词停顿
								str[Morse_len++]=' ';
								Space_num++;
								如果字符串数组最后一个字符不为空格 字符串数组后加一个空格 清零光信号长度 清零储存凯撒代码的数组
							}
						else if(Dark_time > 7)
							{
								结束 删除字符串末尾的空格 处理密码 以要求形式打印字符串
								start_input = 0;
							}
					}
				}
			*/
		}
	}
}

int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
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
