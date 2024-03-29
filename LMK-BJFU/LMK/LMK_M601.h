#ifndef __LMK_M601
#define __LMK_M601

#include "main.h"

#define M601_SINGAL_HIGH()	M610_PIN_GPIO_Port->BSRR = (M610_PIN_Pin <<  0U)
#define M601_SINGAL_LOW()		M610_PIN_GPIO_Port->BSRR = (M610_PIN_Pin << 16U)

//#define M601_SINGAL_HIGH()	TEST_DELAY_GPIO_Port->BSRR = (TEST_DELAY_Pin <<  0U)
//#define M601_SINGAL_LOW()		TEST_DELAY_GPIO_Port->BSRR = (TEST_DELAY_Pin << 16U)





void M601_Reset(void);

//-------------------------------------------------------------

void M601_Init(void);

#endif  //__LMK_M601
