#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "owmy.h"
#include "stdio.h"
#include "adc.h"
#include "stm32f4xx_hal_def.h"

float RFP_value = 0;

float get_value()
{
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1,50);
	if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1),HAL_ADC_STATE_REG_EOC))
	{
		return (float)HAL_ADC_GetValue(&hadc1)/4095.0*3.3f;
	}
	else
		return 0;
}

void RFP602_Task_Main(void const * argument)
{
	
	while(1)
	{
		RFP_value = get_value();
		printf("\r\n testAD=%f",RFP_value);
		osDelay(1000);	
	}
}