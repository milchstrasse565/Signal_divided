/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
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
#include "cmsis_os.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LMK_Delay.h"
#include "stdio.h"
#include "arm_math.h"
#include "arm_const_structs.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void generateFlatTopWindow(float32_t *window, uint32_t size);
#define FLAT_TOP_WINDOW_SIZE 1024
#define FFT_LENGTH 1024
#define SAMPLING_RATE 1000000
#define DAC_BUFFER_SIZE 512
__IO uint8_t AdcConvEnd = 0;
uint16_t adcBuff[FFT_LENGTH];
float32_t window[FFT_LENGTH * 2];
float fft_inputbuf[FFT_LENGTH * 2];  
float fft_outputbuf[FFT_LENGTH];  
//float32_t dacBuffer1[DAC_BUFFER_SIZE];
//float32_t dacBuffer2[DAC_BUFFER_SIZE];
//uint32_t dacValue1[DAC_BUFFER_SIZE];
//uint32_t dacValue2[DAC_BUFFER_SIZE];


uint32_t binIndex1, binIndex2;
float32_t signalFreq1;  // Frequency of first sinusoid (Hz)
float32_t signalFreq2;  // Frequency of second sinusoid (Hz)
float32_t signalAmplitude1;  // Amplitude of first sinusoid
float32_t signalAmplitude2;   // Amplitude of second sinusoid

uint32_t integerPart1;
uint32_t integerPart2;
float32_t decimalPart1;
float32_t decimalPart2;

// Function to apply Hamming window to input data
void generateFlatTopWindow(float32_t *window, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        float32_t a0 = 1.0;
        float32_t a1 = 1.93;
        float32_t a2 = 1.29;
        float32_t a3 = 0.388;
        float32_t a4 = 0.028;
        
        float32_t x = (float32_t)i / (float32_t)(size - 1);
        window[i] = a0 - a1 * cosf(2 * PI * x) + a2 * cosf(4 * PI * x) - a3 * cosf(6 * PI * x) + a4 * cosf(8 * PI * x);
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
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_DAC_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	
//	HAL_TIM_Base_Start_IT(&htim14);
	HAL_TIM_Base_Start(&htim3);                           //开启定时器3
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcBuff, FFT_LENGTH); //让ADC1去采集数，存放到adc_buff数组里
  while (!AdcConvEnd);                                   //等待转换完毕
//	for (uint16_t i = 0; i < FFT_LENGTH; i++)
//{
//    printf("%.3f\n", adcBuff[i] * 3.3 / 4095); //数据打印，查看结果
//}

for (int i = 0; i < FFT_LENGTH; i++)
{
    fft_inputbuf[i * 2] =   adcBuff[i] * 3.3 / 4096;//实部赋值，* 3.3 / 4096是为了将ADC采集到的值转换成实际电压
    fft_inputbuf[i * 2 + 1] = 0;//虚部赋值，固定为0.
}
generateFlatTopWindow(window, FFT_LENGTH * 2);
//arm_mult_f32(fft_inputbuf, window, fft_inputbuf, 2 * FFT_LENGTH);

for (int i = 0; i < FFT_LENGTH * 2; i++) {
    fft_inputbuf[i] *= window[i];
}
arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf, 0, 1);
//此时inputbuffer放的是复数，对其求模放到outputbuffer里
arm_cmplx_mag_f32(fft_inputbuf, fft_outputbuf, FFT_LENGTH);

fft_outputbuf[0] /= 1024;

for (int i = 1; i < FFT_LENGTH; i++)//得到各次谐波幅值
{
    fft_outputbuf[i] /= 512;
}
//频率分离算法
	for (uint16_t i = FFT_LENGTH/2; i < FFT_LENGTH; i++)
{
    fft_outputbuf[i] = 0; 
}
 fft_outputbuf[0] = 0; 
 fft_outputbuf[1] = 0; 
 fft_outputbuf[2] = 0; 
arm_max_f32(fft_outputbuf, FFT_LENGTH, &signalAmplitude1, &binIndex1);
fft_outputbuf[binIndex1] = 0.0f;
 fft_outputbuf[binIndex1-1] = 0; 
 fft_outputbuf[binIndex1-2] = 0; 
 fft_outputbuf[binIndex1+1] = 0; 
 fft_outputbuf[binIndex1+2] = 0; 
arm_max_f32(fft_outputbuf, FFT_LENGTH, &signalAmplitude2, &binIndex2);
signalFreq1 = (float32_t)binIndex1 * (SAMPLING_RATE / FFT_LENGTH);
signalFreq2 = (float32_t)binIndex2 * (SAMPLING_RATE / FFT_LENGTH);
signalFreq1=signalFreq1/1000;
signalFreq2=signalFreq2/1000;

integerPart1 = (uint32_t)floor(signalFreq1);
decimalPart1 = signalFreq1 - integerPart1;
if(decimalPart1>0.5)
{
decimalPart1=1;
}
else
	decimalPart1=0;
signalFreq1=(integerPart1+decimalPart1)*1000;
if(signalFreq1>100000)
{
signalFreq1=1000000-signalFreq1;
}

uint32_t integerPart2 = (uint32_t)floor(signalFreq2);
decimalPart2 = signalFreq2 - integerPart2;
if(decimalPart2>0.5)
{
decimalPart2=1;
}
else
	decimalPart2=0;
signalFreq2=(integerPart2+decimalPart2)*1000;
if(signalFreq2>100000)
{
signalFreq2=1000000-signalFreq2;
}

if(signalFreq1>signalFreq2)
{
float32_t temp;
temp=signalFreq1;
signalFreq1=signalFreq2;
signalFreq2=temp;
}


printf("%.2f\r\n", signalFreq1);
printf("%.2f\r\n", signalFreq2);
printf("%.2f\r\n",signalAmplitude1);
printf("%.2f\r\n",signalAmplitude2);



/*DAC的运行频率为4.2M*/
uint16_t samples1 = 4200000/signalFreq1;
uint16_t samples2 =	4200000/signalFreq2;
uint16_t dacBuffer1[samples1];
uint16_t dacBuffer2[samples2];			


for (int i = 0; i < samples1; i++)
{
dacBuffer1[i]=(uint32_t)((2*signalAmplitude1 * sin(2.0f * PI* i /samples1)+signalAmplitude1*2)*4096/3.3);
		              if (dacBuffer1[i] > 4095) {
                  dacBuffer1[i] = 4095;
                                           }
}

for (int i = 0; i < samples2; i++)
{
dacBuffer2[i]=(uint32_t)((2*signalAmplitude2 * sin(2.0f * PI* i /samples2)+2*signalAmplitude2)*4096/3.3);
		              if (dacBuffer2[i] > 4095) {
                  dacBuffer2[i] = 4095;
                                           }
}

HAL_TIM_Base_Start(&htim2);                           //开启定时器3
HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)dacBuffer1, samples1, DAC_ALIGN_12B_R);
HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t *)dacBuffer2, samples2, DAC_ALIGN_12B_R);
//HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)Sine12bit, 32, DAC_ALIGN_12B_R);


//printf("FFT Result:\r\n");

//for (int i = 0; i < samples2; i++)//输出各次谐波幅值
//{
//	printf("%d \r\n", i, dacBuffer2[i]);
//}



  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
//  MX_FREERTOS_Init();
//  /* Start scheduler */
//  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		HAL_GPIO_TogglePin (LED1_GPIO_Port ,LED1_Pin );
	  HAL_Delay (500);
//		GetTemp();
//		ReadStatusConfig(&status,&cfg);
//		printf(" SC=%2X%2X\r\n", status, cfg);
//		
//		HAL_Delay(1000);
//		M601_SINGAL_HIGH();
//		LMK_Delay_Us(400);
//		M601_SINGAL_LOW();
//		LMK_Delay_Us(200);
		
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
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
