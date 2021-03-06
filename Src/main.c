
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
/* **************** */
/* Global variables */
/* **************** */
/* The initial value given by the ADC before processing */
uint32_t tempReading = 0;
/* Timestamp of the sample, date has day, month and year; time has hours, minutes and seconds*/
RTC_DateTypeDef date;
RTC_TimeTypeDef time;
RTC_DateTypeDef setdate;
RTC_TimeTypeDef settime;
/* value is the final value of the temperature after processing */
float value = 0;
/* message is the line given to the user through the UART with temperature and timestamp */
char message[35] = {0};
uint8_t flag = 0;
/* Decimal point in the timestamp for the fraction of the second */
uint16_t decimal = 0;
//#define prova
#ifdef prova
uint8_t command[2] = {0};
#endif
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void PrintValues(float value, char message[40]);
void GetTemperature(float vin, ADC_HandleTypeDef* hadc);
void GetTimeAndDate(void);
#ifdef prova
void MainMenu(void);
#endif
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */


  HAL_TIM_Base_Start_IT(&htim2);
  HAL_ADC_Start_IT(&hadc1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  /*! We call this function in the while because, in order for the RTC clock to work properly, it must be called continuously, as often as possible.*/
	  GetTimeAndDate();
	  if (flag == 1) {
		  /*! Print everything through UART in the terminal "PrintValues(value, message)"*/
		  	PrintValues(value, message);
		  	flag = 0;
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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Activate the Over-Drive mode 
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* ************** */
/* Local Function */
/* ************** */
/*!
 brief: 	Gets the time and date from the RTC clock and calls the
 	 	 	 printValues function which prints the message in UART
 	 	 	 with temperature and timestamp*/
/*!file:		main.c*/
/*!param:  	None.*/
/*!retval: 	None*/

void GetTimeAndDate(void) {
	/*! Gets the date from the RTC "HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN)"*/
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
	/*! Gets the time from the RTC "HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN)"*/
	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);

}

/* ************** */
/* Local Function */
/* ************** */
/*!
 brief: 	Gets the temperature from the ADC and process it
 	 	 	 in order to have the final value.*/
/*!file:		main.c*/
/*!param:  	None.*/
/*!retval: 	None*/

void GetTemperature(float vin, ADC_HandleTypeDef* hadc) {
	/*! Get the value from the ADC "tempReading = HAL_ADC_GetValue(hadc)"*/
	tempReading = HAL_ADC_GetValue(hadc);
	/*! Conversion of value to volts in float "vin = ((float)tempReading/4095.0)*VREF" */
	vin = ((float)tempReading/4095.0)*VREF;
	/*! Conversion of value to temperature degrees centigrade in float "value = (((float)vin - V25)/SLOPE) + 25.0" */
	value = (((float)vin - V25)/SLOPE) + 25.0;
}

/* ************** */
/* Local Function */
/* ************** */
/*!
  * @brief 	Print a value of the temperature and its timestamp
  * @file	main.c
  * @param  value:		the value of the temperature in degrees Celsius
  * @param  message:	the message printed on the console with temperature and timestamp
  * @retval None
  */


void PrintValues(float value, char message[40]) {
	/*! Put the value of the temperature with timestamp into the message variable "sprintf(message, "T = %u C - %02d:%02d:%02d.%u - %02u %02u %02u \n\r", (int)value, time.Hours, time.Minutes, time.Seconds, decimal%2*5, date.Date, date.Month, date.Year)"*/
	sprintf(message, "T = %u C - %02d:%02d:%02d.%u - %02u %02u %02u \n\r", (int)value, time.Hours, time.Minutes, time.Seconds, decimal%2*5, date.Date, date.Month, date.Year);
	/*! Transmit the value of the temperature with timestamp to the console through the UART peripheral "HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 100)" */
	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 100);
}

/* ************** */
/* Local Function */
/* ************** */
/*!
 brief: 	Regular conversion complete callback in non blocking mode.
 			Definition of the HAL_ADC_ConvCpltCallback function.
			This function is called when the ADC has finished converting*/
/*!file:		Drivers\STM32F4xx_HAL_Driver\Inc\stm32f4xx_hal_adc.c*/
/*!param:  	hadc pointer to a ADC_HandleTypeDef structure that contains
         	the configuration information for the specified ADC.*/
/*!retval: 	None*/

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	float vin = 0;
	/*! Every time the ADC is ready we call GetTemperature to process the ADC data and have the temperature "GetTemperature(vin, hadc)"*/
	GetTemperature(vin, hadc);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
