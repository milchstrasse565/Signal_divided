
/****************************************************************************************/
/*
 *
 * Copyright (C) 2020. Mysentech Inc, unpublished work. This computer 
 * program includes Confidential, Proprietary Information and is a Trade Secret of 
 * Minyuan Sensing Technology Inc.(Mysentech)  All use, disclosure, and/or reproduction is prohibited 
 * unless authorized in writing. All Rights Reserved.
 *
 *  Please contact <sales@mysentech.com> or contributors for further questions.
*/
/****************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "owmy.h"
//#include "bsp_SysTick.h"
#include "LMK_Delay.h"
#include "M601.h"
#include <math.h>
#include "MY_stdtype.h"
#include <stdbool.h>
#include "stdio.h"

/**
  * @brief  把16位二进制补码表示的温度输出转换为以摄氏度为单位的温度读数
  * @param  out：有符号的16位二进制温度输出
  * @retval 以摄氏度为单位的浮点温度
*/
float M601_OutputtoTemp(int16_t out)
{
	return ((float)out/256.0 + 40.0);	
}

/**
  * @brief  把以摄氏度为单位的浮点温度值转换为16位二进制补码表示的温度值
  * @param  以摄氏度为单位的浮点温度值
  * @retval 有符号的16位二进制温度值
*/
int16_t M601_TemptoOutput(float Temp)
{
	return (int16_t)((Temp-40.0)*256.0);	
}

/**
  * @brief  计算多个字节序列的校验和
  * @param  serial：字节数组指针
  * @param  length：字节数组的长度
  * @retval 校验和（CRC）
*/
#define POLYNOMIAL 	0x131 //100110001
uint8_t MY_OW_CRC8(uint8_t *serial, uint8_t length) 
{
    uint8_t result = 0x00;
    uint8_t pDataBuf;
    uint8_t i;

    while(length--) {
        pDataBuf = *serial++;
        for(i=0; i<8; i++) {
            if((result^(pDataBuf))&0x01) {
                result ^= 0x18;
                result >>= 1;
                result |= 0x80;
            }
            else {
                result >>= 1;
            }
            pDataBuf >>= 1;
        }
    }
		
    return result;
}

BOOL OW_ReadRomCode_SkipRom(uint8_t *romcode)  
{
  uint8_t i;

	if(OW_ResetPresence() == FALSE)
		return FALSE;
	
	OW_WriteByte(READ_ROM); 
	
	for(i=0; i < sizeof(M601_ROMCODE); i++) 
     *romcode++ = OW_ReadByte(); 
	
	return TRUE;
}

BOOL M601_nReadScratchpad_SkipRom(uint8_t *scr, uint8_t size)
{
    int16_t i;

	/*size < sizeof(M601_SCRATCHPAD_READ)*/
    if(OW_ResetPresence() == FALSE)						
			return FALSE;
		
    OW_WriteByte(SKIP_ROM);
    OW_WriteByte(READ_SCRATCHPAD);
		
		for(i=0; i<size; i++)
    {
			*scr++ = OW_ReadByte();
		}

    return TRUE;
}

/**
  * @brief  读芯片寄存器的暂存器组
  * @param  scr：字节数组指针， 长度为 @sizeof（M601_SCRATCHPAD_READ）
  * @retval 读状态
*/
BOOL M601_ReadScratchpad_SkipRom(uint8_t *scr)
{
    int16_t i;

	/*size < sizeof(M601_SCRATCHPAD_READ)*/
    if(OW_ResetPresence() == FALSE)					
			return FALSE;
		
    OW_WriteByte(SKIP_ROM);
    OW_WriteByte(READ_SCRATCHPAD);
		
		for(i=0; i < sizeof(M601_SCRATCHPAD_READ); i++)
    {
			*scr++ = OW_ReadByte();
		}

    return TRUE;
}
/**
  * @brief  写芯片寄存器的暂存器组
  * @param  scr：字节数组指针， 长度为 @sizeof（M601_SCRATCHPAD_WRITE）
  * @retval 写状态
**/
BOOL M601_WriteScratchpad_SkipRom(uint8_t *scr)
{
    int16_t i;

    if(OW_ResetPresence() == FALSE)						
			return FALSE;
		
    OW_WriteByte(SKIP_ROM);
    OW_WriteByte(WRITE_SCRATCHPAD);
		
		for(i=0; i < sizeof(M601_SCRATCHPAD_WRITE); i++)
    {
			OW_WriteByte(*scr++);
		}

    return TRUE;
}

/**
  * @brief  启动温度测量
  * @param  无
  * @retval 单总线发送状态
*/
BOOL ConvertTemp(void)
{	 
	if(OW_ResetPresence() == FALSE)					
		return FALSE;

  OW_WriteByte(SKIP_ROM);
  OW_WriteByte(CONVERT_T);
		
  return TRUE;
}


/**
  * @brief  等待转换结束后读测量结果。和@ConvertTemp联合使用
  * @param  iTemp：返回的16位温度测量结果
  * @retval 读状态
*/
BOOL ReadTempWaiting(uint16_t *iTemp)
{
	uint8_t scrb[sizeof(M601_SCRATCHPAD_READ)];
	M601_SCRATCHPAD_READ *scr = (M601_SCRATCHPAD_READ *) scrb;

	/*读9个字节。前两个是温度转换结果，最后字节是前8个的校验和--CRC。*/	
	if(M601_ReadScratchpad_SkipRom(scrb) == FALSE)
	{		
		return FALSE;  /*读寄存器失败*/
	}

	/*计算接收的前8个字节的校验和，并与接收的第9个CRC字节比较。*/	

  if(scrb[8] != MY_OW_CRC8(scrb, 8))
  {	
		return FALSE;  /*CRC验证未通过*/
  }

	/*将温度测量结果的两个字节合成为16位字。*/	
	*iTemp=(uint16_t)scr->T_msb<<8 | scr->T_lsb; 
	
  return TRUE;		
}

/**
  * @brief  查询是否转换结束，然后读测量结果。和@ConvertTemp联合使用
  * @param  iTemp：返回的16温度测量结果
  * @retval 读结果状态
*/
BOOL ReadTempPolling(uint16_t *iTemp)
{ int timeout = 0;
		
	/*读状态位时隙。如果转换还没结束，芯片以1响应读时隙。如果转换结束，芯片以0响应度时隙。
	前两个字节是温度转换结果，最后字节是前8个的校验和--CRC。*/		
	while (OW_ReadStatus() == BUSY )
	{	
		DELAY_Ms(1);	
    timeout++; 
		if(timeout > 50) 
		{				
			return FALSE;				/*超时错误*/
		}
	}
	
	uint8_t scrb[sizeof(M601_SCRATCHPAD_READ)];
	M601_SCRATCHPAD_READ *scr = (M601_SCRATCHPAD_READ *) scrb;
	
	/*读9个字节。前两个是温度转换结果，最后字节是前8个的校验和--CRC。*/	
	if(M601_ReadScratchpad_SkipRom(scrb) == FALSE)
	{		
		return FALSE;  /*I2C地址头应答为NACK*/
	}

	/*计算接收的前8个字节的校验和，并与接收的第9个CRC字节比较。*/	
  if(scrb[8] != MY_OW_CRC8(scrb, 8))
  {	
		return FALSE;  /*CRC验证未通过*/
  }
	/*将温度测量结果的两个字节合成为16位字。*/				
	*iTemp=(uint16_t)scr->T_msb<<8 | scr->T_lsb;
	
  return TRUE;		
}

/**
  * @brief  设置周期测量频率和重复性
  * @param  mps 要设置的周期测量频率（每秒测量次数），可能为下列其一
	*				@arg CFG_MPS_Single		：每执行ConvertTemp一次，启动一次温度测量
	*				@arg CFG_MPS_Half			：每执行ConvertTemp一次，启动每秒0.5次重复测量
	*				@arg CFG_MPS_1				：每执行ConvertTemp一次，启动每秒1次重复测量
	*				@arg CFG_MPS_2				：每执行ConvertTemp一次，启动每秒2次重复测量
	*				@arg CFG_MPS_4				：每执行ConvertTemp一次，启动每秒4次重复测量
	*				@arg CFG_MPS_10				：每执行ConvertTemp一次，启动每秒10次重复测量
  * @param  repeatability：要设置的重复性值，可能为下列其一
	*				@arg CFG_Repeatbility_Low				：设置低重复性
	*				@arg CFG_Repeatbility_Medium		：设置中重复性
	*				@arg CFG_Repeatbility_High			：设置高重复性
  * @retval 无
*/
BOOL SetConfig(uint8_t mps, uint8_t repeatability)
{ 
	uint8_t scrb[sizeof(M601_SCRATCHPAD_READ)];
	M601_SCRATCHPAD_READ *scr = (M601_SCRATCHPAD_READ *) scrb;

	/*读9个字节。第7字节是系统配置寄存器，第8字节是系统状态寄存器。最后字节是前8个的校验和--CRC。*/	
	if(M601_ReadScratchpad_SkipRom(scrb) == FALSE)
	{	
		return FALSE;  /*读暂存器组水平*/
	}

	/*计算接收的前8个字节的校验和，并与接收的第9个CRC字节比较。*/	
  if(scrb[8] != MY_OW_CRC8(scrb, 8))
  {			
		return FALSE;  /*CRC验证未通过*/
  }
	
	scr->Cfg &= ~CFG_Repeatbility_Mask;
	scr->Cfg |= repeatability;
	scr->Cfg &= ~CFG_MPS_Mask;
	scr->Cfg |= mps;		
	
	M601_WriteScratchpad_SkipRom(scrb+4);
	
	return TRUE;
}

/**
  * @brief  读状态和配置
  * @param  status 返回的状态寄存器值
  * @param  cfg 返回的配置寄存器值
  * @retval 状态
*/
BOOL ReadStatusConfig(uint8_t *status, uint8_t *cfg)
{ 
	uint8_t scrb[sizeof(M601_SCRATCHPAD_READ)];
	M601_SCRATCHPAD_READ *scr = (M601_SCRATCHPAD_READ *) scrb;

	/*读9个字节。第7字节是系统配置寄存器，第8字节是系统状态寄存器。最后字节是前8个的校验和--CRC。*/		
	if(M601_ReadScratchpad_SkipRom(scrb) == FALSE)
	{
		return FALSE;  /*CRC验证未通过*/
	}

	/*计算接收的前8个字节的校验和，并与接收的第9个CRC字节比较。*/	
  if(scrb[8] != MY_OW_CRC8(scrb, 8))
  {		
		return FALSE;  /*CRC验证未通过*/
  }	
	
	*status = scr->Status;
	*cfg = scr->Cfg;	
	
	return TRUE;
}

/**
  * @brief  启动温度测量并读温
  * @param  无
  * @retval 状态
*/

float M601Temp = 0;

BOOL  GetTemp(void)
{ 
	uint16_t iTemp;
	float fTemp;
	if(ConvertTemp() == FALSE) return FALSE;
	DELAY_Ms(12);
	ReadTempWaiting(&iTemp);		
	fTemp=M601_OutputtoTemp((int16_t)iTemp);
	M601Temp = fTemp;
	//printf("\r\n M601 Temp: %.3f ", fTemp);
	return TRUE;
}

/**********************************************************************************************/
