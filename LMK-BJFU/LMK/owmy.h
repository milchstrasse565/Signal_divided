/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef OWMY_H
#define OWMY_H

/* Includes ------------------------------------------------------------------*/
//#include "HAL_device.h"
//#include "hal_conf.h"
//#include "types.h"
#include "MY_stdtype.h"
#include "LMK_M601.h"
#include "main.h"


/*********用户可自行配置us级延时************/

//#define ow_Delay_us			Delay_us
#define DELAY_Ms(x)			HAL_Delay(x)

/* Definition of DQ pin for one-wire communication*/

#define GPIOOW_DQ_GPIO_PORT          		M610_PIN_GPIO_Port
#define GPIOOW_DQ_PIN                   M610_PIN_Pin
#define GPIOOW_DQ_BUSCLK             		RCC_APB2Periph_GPIOB
#define GPIOOW_DQ_GPIO_CLK_ENABLE()  		RCC_APB2PeriphClockCmd(GPIOOW_DQ_BUSCLK, ENABLE);

#define GPIOOW_VDD_GPIO_PORT          	GPIOA
#define GPIOOW_VDD_PIN                  GPIO_Pin_12
#define GPIOOW_VDD_BUSCLK             	RCC_APB2Periph_GPIOA
#define GPIOOW_VDD_GPIO_CLK_ENABLE()  	RCC_APB2PeriphClockCmd(GPIOOW_DQ_BUSCLK, ENABLE);


/* Macros for DQ manipulation*/
#define ow_DQ_set()   		M601_SINGAL_HIGH()
#define ow_DQ_reset() 		M601_SINGAL_LOW()
#define ow_DQ_get()   		( M610_PIN_GPIO_Port->IDR & M610_PIN_Pin )						//等待测试

#define ow_VDD_set()   		{ GPIOOW_VDD_GPIO_PORT->BSRR = GPIOOW_VDD_PIN; }
#define ow_VDD_reset() 		{ GPIOOW_VDD_GPIO_PORT->BRR = GPIOOW_VDD_PIN;  }

typedef enum {
  READY       = 0,
  BUSY    		= 1
} OW_SLAVESTATUS;

/* Exported_Functions----------------------------------------------------------*/
void OW_Init(void);
BOOL OW_ResetPresence(void);
void OW_WriteByte(uint8_t data);
uint8_t OW_ReadByte(void);
OW_SLAVESTATUS OW_ReadStatus(void);

uint8_t OW_Read2Bits(void);


#endif /* OWMY_H */
