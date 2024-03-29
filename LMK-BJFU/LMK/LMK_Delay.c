//#include "LMK_Delay.h"
//#include "tim.h"

//static volatile uint32_t delayCount = 0;

//void LMK_Delay_Us(uint32_t us)						//usÑÓÊ±
//{
//	delayCount = (uint32_t)(us >> 1);	
//	
//	TIM14->CNT = 0;
//	HAL_TIM_Base_Start_IT(&htim14);
//	while(delayCount != 0); 
//}


//void LMK_Delay_TIM_Handle(void)
//{
//	if (delayCount != 0x00)
//	{ 
//		delayCount--;
//	}
//}


