#include "LMK_M601.h"

#include "LMK_Delay.h"

/******One-wire communication timing requirements*******/
//SPON: <=10 us. Time to Strong Pullup On.
//tSLOT: >=60 us, <=120us. Time Slot
//tREC: >=1 us. Recovery Time.
//tLOW0: >=60 us, <=120us. Write 0 Low Time.
//tLOW1: >=1 us, <=15us. Write 1 Low Time.
//tRDV: <=15 us. Read Data Valid.
//tRSTH: >=480 us. Reset Time High.
//tRSTL: >=480 us. Reset Time Low. 
//PDHIGH: >=15 us, <=60us. Presence-Detect High.
//tPDLOW: >=60 us, <=240us. Presence-Detect Low.

/*Time delays in us for GPIO simulated master One-wire communication*/
#define 				tSlot										60				/*us*/
#define					tRecover								10				/*us*/
#define					tInitSlot								3					/*us*/
#define 				tLow_Write_1						tInitSlot
#define 				tHigh_Write_1						50
#define 				tLow_Write_0						53
#define 				tHigh_Write_0						tRecover
#define 				tLow_Read								tInitSlot
#define 				tSample_Read						10     		/*us*/
#define 				tComplement_Read				55
#define 				tLow_Reset							480				/*us*/
#define					tHigh_Reset							480				/*us*/
#define					tSample_Presence				80				/*us*/
#define					tComplement_Presence		(tHigh_Reset-tSample_Presence)


void M601_Reset(void)				//¸´Î»480us
{
	M601_SINGAL_LOW(); 				// Drive DQ low
	LMK_Delay_Us(tLow_Reset);	
	M601_SINGAL_HIGH(); 			// Release DQ
}

void M601_Init(void)
{
	M601_SINGAL_HIGH();
	HAL_Delay(1);
	M601_Reset();
}

