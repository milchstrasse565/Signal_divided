/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef M601_H
#define M601_H

/* Includes ------------------------------------------------------------------*/
//#include "HAL_device.h"

/* M601/01 Registers definition----------------------------------------------*/
/*Bit definition of config register*/
#define CFG_CLKSTRETCH_Mask   	0x20
#define CFG_MPS_Mask   		  		0x1C
#define CFG_Repeatbility_Mask 	0x03

#define CFG_ClkStreatch_Disable (0x00 << 5)
#define CFG_ClkStreatch_Enable 	(0x01 << 5)

#define CFG_MPS_Single  	0x00
#define CFG_MPS_Half  		0x04
#define CFG_MPS_1  			0x08
#define CFG_MPS_2 			0x0C
#define CFG_MPS_4 			0x10
#define CFG_MPS_10 			0x14

#define CFG_Repeatbility_Low   	0x00
#define CFG_Repeatbility_Medium 0x01
#define CFG_Repeatbility_High 	0x02

/*Bit definition of status register*/
#define Status_Meature_Mask   	0x81
#define Status_WriteCrc_Mask   	0x20
#define Status_CMD_Mask   			0x10
#define Status_POR_Mask   			0x08

/**************  Bit definition for M601 configuration register  *********/
#define M601_CFG_I2CCLKSTRETCH_MASK          0x20
#define M601_CFG_MPS_MASK										0x1C
#define M601_CFG_REPEATABILITY_MASK          0x03
/******************  Bit definition for M601 temperature register  *******/
#define M601_REPEATABILITY_LOW               (0x00 << 0)
#define M601_REPEATABILITY_MEDIUM            (0x01 << 0)
#define M601_REPEATABILITY_HIGH              (0x02 << 0)
/******************  Bit definition for TTrim in parameters  *******/
#define M601_MPS_SINGLE					            (0x00 << 2)
#define M601_MPS_0P5Hz					            	(0x01 << 2)
#define M601_MPS_1Hz					            		(0x02 << 2)
#define M601_MPS_2Hz					            		(0x03 << 2)
#define M601_MPS_4Hz					            		(0x04 << 2)
#define M601_MPS_10Hz					            	(0x05 << 2)

#define M601_CLKSTRETCH_EN					          (0x01 << 5)
/******************  Bit definition for status register  *******/
#define M601_STATUS_CONVERTMODE_MASK          0x81
#define M601_STATUS_I2CDATACRC_MASK           0x20
#define M601_STATUS_I2CCMDCRC_MASK            0x10
#define M601_STATUS_SYSRESETFLAG_MASK         0x08

#define M601_CONVERTMODE_IDLE             		 0x00
#define M601_CONVERTMODE_T             		   0x01

/*Definition of conversion time corresponding to repeatability setting*/
#define tTempConvert_LowRepeatability   		4		/* ms. */
#define tTempConvert_MediumRepeatability   	6		/* ms. */
#define tTempConvert_HighRepeatability   		11	/* ms. */

/*Temperature conversion time for different resolutions */
#define M601_tConv_T_LOWREPEATABILITY		  3.5		/*ms*/
#define M601_tConv_T_MEDIUMREPEATABILITY		  93.75		/*ms*/
#define M601_tConv_T_HIGHREPEATABILITY		  10.5		/*ms*/
#define M601_tConv_C_LOWREPEATABILITY		  3.5		/*ms*/
#define M601_tConv_C_MEDIUMREPEATABILITY		  93.75		/*ms*/
#define M601_tConv_C_HIGHREPEATABILITY		  10.5		/*ms*/
/* M601/01 ow Commands-------------------------------------------------------*/
typedef enum
{
	//ROM command
  SKIP_ROM            	= 0xcc,
  READ_ROM            	= 0x33,
  MATCH_ROM           	= 0x55,
	SEARCH_ROM           	= 0xf0, 
	ALARM_SEARCH					= 0xec,
	//Function command
  CONVERT_T           	= 0x44,
	READ_SCRATCHPAD     	= 0xbe,
	WRITE_SCRATCHPAD     	= 0x4e,	
} M601_OW_CMD;

/******************  Scratchpad/SRAM  ******************/
/*SRAM scratchpad*/
typedef struct
{
	uint8_t T_lsb;					/*The LSB of 温度结果, RO*/
	uint8_t T_msb;					/*The MSB of 温度结果, RO*/
	uint8_t C1_lsb;					/*The LSB of 电容通道C1, RO*/
	uint8_t C1_msb;					/*The MSB of 电容通道C1, Ro*/	
	uint8_t Tha_set_lsb;		
	uint8_t Tla_set_lsb;		
	uint8_t Cfg;						/*系统配置寄存器, RW*/
	uint8_t Status;					/*系统状态寄存器, RO*/
	uint8_t crc_scr;				/*CRC for byte0-7, RO*/
} M601_SCRATCHPAD_READ;

typedef struct
{	
	int8_t Tha_set_lsb;				
	int8_t Tla_set_lsb;			
	uint8_t Cfg;						/*系统配置寄存器, RW*/
} M601_SCRATCHPAD_WRITE;

typedef struct
{
	uint8_t tha_clear;				
	uint8_t tla_clear;					
	uint8_t hha_set;					
	uint8_t hla_set;					
	uint8_t hha_clear;				
	uint8_t hla_clear;					
	uint8_t udf[5];							
	uint8_t MPW_test;					
	uint8_t crc_ext;						
} M601_SCRATCHPADEXT;

typedef struct
{
	uint8_t Family;				/*Family byte, RO*/
	uint8_t Id[6];				/*Unique ID, RO*/
	uint8_t crc_rc;				/*Crc code for byte0-7, RO*/
} M601_ROMCODE;

typedef struct
{
	uint8_t Res[3];
	uint8_t Ch_Sel;					
	uint8_t Cos;						
	uint8_t Res1;				
	uint8_t T_coeff[3];			
	uint8_t Cfb;															
	uint8_t Res2;
	uint8_t Res3[2];
	uint8_t dummy8;
	uint8_t crc_para;				/*CRC for byte0-13, RO*/
} M601_SCRPARAMETERS;

/*Exported functions*/
float M601_OutputtoTemp(int16_t out);
int16_t M601_TemptoOutput(float Temp);
BOOL ConvertTemp(void);
BOOL ReadTempWaiting(uint16_t *iTemp);
BOOL ReadTempPolling(uint16_t *iTemp);
BOOL  GetTemp(void);
//BOOL  GetTemp(float *fTemp);

BOOL ReadStatusConfig(uint8_t *status, uint8_t *cfg);
BOOL SetConfig(uint8_t mps, uint8_t repeatability);

BOOL TempOffsetCal(float ta, float tr);
BOOL ReadTempCal(float *tslope, float *toffset);


/******************  For test  ******************/
void M601_ReadAllScratch(void);
BOOL set_powerdown(uint8_t set);

#endif /*M601_OW_H */
