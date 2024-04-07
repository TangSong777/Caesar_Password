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
#define Bright GPIO_PIN_RESET
#define Dark GPIO_PIN_SET
/* USER CODE BEGIN PTD */
typedef enum
{
	OUTPUT = 0,
	EZINPUT = 1,
	INPUT = 2
} PUT_STATE; // 三种模式(通过长按切换)

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
PUT_STATE PutState = OUTPUT; // 默认为输出信号模式
LED_STATE LedState = mode1;
uint8_t Key_flag = 0;	   // 按键标志定义
uint32_t Bright_time = 0;  // 代表亮起的单位时间
uint32_t Dark_time = 0;	   // 代表熄灭的单位时间
uint8_t Start_ezinput = 0; // 定义开始接受标志
uint8_t Start_input = 0;   // 定义开始接受标志
uint8_t Morse_len = 0;	   // 定义摩尔斯密码长度
uint8_t Str_len = 0;	   // 定义字符串长度
uint8_t Space_num = 0;	   // 定义字符串中的空格数
uint8_t T = 0;			   // 定义t
uint8_t Morse[50] = {0};   // 定义数组储存摩尔斯密码
uint8_t Str[200] = {0};	   // 定义数组储存字符串
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

void Clear_array(uint8_t morse[], uint8_t morse_len)
{
	for (int temp = 0; temp < morse_len; temp++)
		morse[temp] = 0;
}

void Transform_password(uint8_t str[], uint8_t str_len, uint8_t t)
{
	for (int i = 0; i < str_len; i++)
	{
		if (str[i] >= 'a' && str[i] <= 'z')
		{
			str[i] = (str[i] - 'a' - t + 26) % 26 + 'a';
		}
	};
}

int Judge(uint8_t data[], const char a[], int len)
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

void Transform(uint8_t morse[], uint8_t str[], uint8_t morse_len, uint8_t str_len)
{
	switch (morse_len)
	{
	case 1:
	{
		if (Judge(morse, ".", 1))
			str[str_len] = 'e';
		if (Judge(morse, "-", 1))
			str[str_len] = 't';
		break;
	}
	case 2:
	{
		if (Judge(morse, ".-", 2))
			str[str_len] = 'a';
		if (Judge(morse, "..", 2))
			str[str_len] = 'i';
		if (Judge(morse, "--", 2))
			str[str_len] = 'm';
		if (Judge(morse, "-.", 2))
			str[str_len] = 'n';
		break;
	}
	case 3:
	{
		if (Judge(morse, "-..", 3))
			str[str_len] = 'd';
		if (Judge(morse, "--.", 3))
			str[str_len] = 'g';
		if (Judge(morse, "-.-", 3))
			str[str_len] = 'k';
		if (Judge(morse, "---", 3))
			str[str_len] = 'o';
		if (Judge(morse, ".-.", 3))
			str[str_len] = 'r';
		if (Judge(morse, "...", 3))
			str[str_len] = 's';
		if (Judge(morse, "..-", 3))
			str[str_len] = 'u';
		if (Judge(morse, ".--", 3))
			str[str_len] = 'w';
		break;
	}
	case 4:
	{
		if (Judge(morse, "-...", 4))
			str[str_len] = 'b';
		if (Judge(morse, "-.-.", 4))
			str[str_len] = 'c';
		if (Judge(morse, "..-.", 4))
			str[str_len] = 'f';
		if (Judge(morse, "....", 4))
			str[str_len] = 'h';
		if (Judge(morse, ".---", 4))
			str[str_len] = 'j';
		if (Judge(morse, ".-..", 4))
			str[str_len] = 'l';
		if (Judge(morse, ".--.", 4))
			str[str_len] = 'p';
		if (Judge(morse, "--.-", 4))
			str[str_len] = 'q';
		if (Judge(morse, "...-", 4))
			str[str_len] = 'v';
		if (Judge(morse, "-..-", 4))
			str[str_len] = 'x';
		if (Judge(morse, "-.--", 4))
			str[str_len] = 'y';
		if (Judge(morse, "--..", 4))
			str[str_len] = 'z';
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
		if (PutState == OUTPUT) // 当模式为发送信号模式时
		{
			if (Key_flag == 1)
			{
				if (LedState < mode3)
					LedState++;
				else
					LedState = 0;
			}
			else if (Key_flag == 3)
				PutState = EZINPUT; // 长按切换为接受信号模式

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
		switch (PutState)
		{
		case OUTPUT:
		{
			Key_pressscan(KEY0, &Key_flag);
			break;
		}
		case EZINPUT:
		{
			Key_pressscan(KEY0, &Key_flag);
			if (HAL_GPIO_ReadPin(Light_GPIO_Port, Light_Pin) == Bright)
			{
				Bright_time++;
				Start_ezinput = 1;
			}
			if (HAL_GPIO_ReadPin(Light_GPIO_Port, Light_Pin) == Dark && Start_ezinput)
				Dark_time++;
			if (Bright_time == Dark_time)
			{
				switch (Bright_time)
				{
				case 100:
				{
					printf("Fight!\r\n");
					Bright_time = 0;
					Dark_time = 0;
					Start_ezinput = 0;
					break;
				}
				case 200:
				{
					printf("Retreat!\r\n");
					Bright_time = 0;
					Dark_time = 0;
					Start_ezinput = 0;
					break;
				}
				case 300:
				{
					printf("Come to me!\r\n");
					Bright_time = 0;
					Dark_time = 0;
					Start_ezinput = 0;
					break;
				}
				}
			}
			break;
		}

		case INPUT:
		{
			if (HAL_GPIO_ReadPin(Light_GPIO_Port, Light_Pin) == Bright)
			{
				if (Start_input == 0)
					Start_input = 1;
				Bright_time++;
				Dark_time = 0;
			}
			else if (HAL_GPIO_ReadPin(Light_GPIO_Port, Light_Pin) == Dark && start_input)
			{
				Dark_time++;
				switch (Dark_time)
				{
				case 1:
				{
					if (Bright_time == 1)
					{
						Morse[Morse_len++] = '.';
						Bright_time = 0;
					}
					else if (Bright_time == 3)
					{
						Morse[Morse_len++] = '-';
						Bright_time = 0;
					}
					break;
				}
				case 3:
				{
					transform(Morse, Str, Morse_len, Str_len++);
					Clear_array(Morse, Morse_len);
					Morse_len = 0;
				}
				case 7:
				{
					Str[Str_len++] = ' ';
					Space_num++;
				}
				}
				if (Dark_time > 7)
				{
					str[Morse_len - 1] = 0;
					T = (Str_len - Space_num) % 7;
					Transform_password(Str, Str_len, T);
					for (int i = 0; i < Str_len - 2; i++)
					{
						printf("%c", Str[i]);
					}
					printf(".\r\n");
					Start_input = 0;
				}
			}
		}
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
