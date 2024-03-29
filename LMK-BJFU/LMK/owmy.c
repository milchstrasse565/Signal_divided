/* Includes ------------------------------------------------------------------*/
#include "owmy.h"
#include "MY_stdtype.h"
//#include "bsp_SysTick.h"
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
//---------------------------------------------------------------------------



void OW_Init(void)
{
//  GPIO_InitTypeDef gpioinitstruct;

//  GPIOOW_DQ_GPIO_CLK_ENABLE();
//	
//  gpioinitstruct.GPIO_Pin 	= GPIOOW_DQ_PIN;
//  gpioinitstruct.GPIO_Speed = GPIO_Speed_10MHz;	
//  gpioinitstruct.GPIO_Mode 	= GPIO_Mode_Out_OD;	
//  GPIO_Init(GPIOOW_DQ_GPIO_PORT, &gpioinitstruct);
//	
//  gpioinitstruct.GPIO_Pin 	= GPIOOW_VDD_PIN;
//  gpioinitstruct.GPIO_Speed = GPIO_Speed_10MHz;	
//  gpioinitstruct.GPIO_Mode 	= GPIO_Mode_Out_PP;	
//  GPIO_Init(GPIOOW_DQ_GPIO_PORT, &gpioinitstruct);	
	
	ow_DQ_set();
	//ow_VDD_set();
}


// OW 'RESET+PRESENSE' timming sequence
//
//      |     >=480us   |        >=480us             |
//------                 --------\                ------------
//      |               /        |              /
//      | (>480us)	    |	15-60us|   60-240us   |
//      |_ _ _ _ _ _ _ _|        | _ _ _ _ _ _ _|

void OW_Reset(void)
{
	ow_DQ_reset(); 	// Drive DQ low
	LMK_Delay_Us(tLow_Reset);	
	ow_DQ_set(); 		// Release DQ
}

BOOL OW_Presence(void)
{
	uint8_t dq;int count=0;
	LMK_Delay_Us(tSample_Presence);
	dq = ow_DQ_get(); 			// Get presence pulse from slave
	while(dq&&(count<17))
  {
    LMK_Delay_Us(10);
	  dq = ow_DQ_get(); 			// Get presence pulse from slave
    count++;
  }
	LMK_Delay_Us(tComplement_Presence); // Complete the reset sequence recovery
	
	return (dq ? FALSE : TRUE); 				
}

BOOL OW_ResetPresence(void)
{
	uint8_t dq;int count=0;
	
	ow_DQ_reset(); 	// Drive DQ low
	LMK_Delay_Us(tLow_Reset);	
	ow_DQ_set(); 		// Release DQ	

	LMK_Delay_Us(tSample_Presence);//After detecting the rising edge on the DQ pin, 
	                              //the M601 waits for 15~60 us and then transmits 
	                              //the presence pulse (a low signal for 60~240us).
	dq = ow_DQ_get(); 			// Get presence pulse from slave
	while(dq&&(count<17))
  {
    LMK_Delay_Us(10);
	  dq = ow_DQ_get(); 			// Get presence pulse from slave
    count++;
  }
	LMK_Delay_Us(tComplement_Presence); 	// Complete the reset-presensce
	
	return (dq ? FALSE : TRUE); 
}

//---------------------------------------------------------------------------
//   MASTER WRITE 0 and WRITE 1 SLOT=70us.
//   |>1us|       60<Tx0<120us         |1us<tRec|         
//    ----                              ----------
//  /     |                            /     
//  |     | 	      |     45us      |  |   
//  |_ _ _|_ _ _ _______ _ _ _ _____ __|  
//   			|   15us  | Slave samples | 

//   |>1us|       60<Tx1<120us         |            
//    ----       ---------------------------------
//  /     |     /                   |      
//  |     |>1us	|   |     45us      |	
//  |_ _ _|_ __ | _______ _ _ _ ___ |  
//  		|   15us  | Slave samples | 

// Send a bit to DQ. Provide 10us recovery time.
void OW_WriteBit(uint8_t bit)
{
	if (bit)
	{
		// Write '1' to DQ
		ow_DQ_reset(); 							// Initialte write '1' time slot.
		LMK_Delay_Us(tLow_Write_1);
		ow_DQ_set(); 		
		LMK_Delay_Us(tHigh_Write_1); // Complete the write '1' time slot.
	}
	else
	{
		// Write '0' to DQ
		ow_DQ_reset();  						// Initialte write '0' time slot
		LMK_Delay_Us(tLow_Write_0);
		ow_DQ_set(); 		
		LMK_Delay_Us(tHigh_Write_0); // Complete the write '0' time slot: recovery
	}
}

//---------------------------------------------------------------------------
// Read a bit from DQ. Provide 10us recovery time.
//
int OW_ReadBit(void)
{
	int bit;
	
	ow_DQ_reset(); 								// Initialte read time slot
	LMK_Delay_Us(tLow_Read);
	ow_DQ_set(); 			
	LMK_Delay_Us(tSample_Read);
	bit = ow_DQ_get(); 				 		// Sample DQ to get the bit from the slave	
	LMK_Delay_Us(tComplement_Read); // Complete the read time slot with 10us recovery
	
	return (bit != 0);
}

/*Send a byte to DQ. LSB first, MSB last.*/
void OW_WriteByte(uint8_t data)
{
	int bit;

	for (bit = 0; bit < 8; bit++)
	{
		OW_WriteBit(data & 0x01);
		data >>= 1;
	}
}
//---------------------------------------------------------------------------
// Read a byte from DQ and return it. LSB first, MSB last.
//
uint8_t OW_ReadByte(void)
{
	uint8_t bit, byte=0;
	
	for (bit = 0; bit < 8; bit++)
	{
		byte >>= 1;
		if (OW_ReadBit())
			byte |= 0x80;
	}
	
	return byte;
}

/* Single read time slot for polling slave ready.*/
OW_SLAVESTATUS OW_ReadStatus(void)
{
	int status;
	
	ow_DQ_reset(); 				         // Initiate read time slot
	LMK_Delay_Us(tLow_Read);
	ow_DQ_set();  
	LMK_Delay_Us(tSample_Read);
	status = ow_DQ_get();          // Get the status from DQ: '0' busy, '1' idle.
	LMK_Delay_Us(tComplement_Read); // Complete the read time slot and recovery
	
	return (status ? READY : BUSY);
}
//---------------------------------------------------------------------------
//Multi-Drop 1-Wire network function: get a bit value and its complement.
//---------------------------------------------------------------------------
uint8_t OW_Read2Bits(void)  
{    
		uint8_t i, dq, data;	
	  data = 0;
	
		for(i=0; i<2; i++) 
		{
			dq = OW_ReadBit();		
			data = (data) | (dq<<i);
		}
	
		return data;
}

