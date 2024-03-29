
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
  * @brief  ��16λ�����Ʋ����ʾ���¶����ת��Ϊ�����϶�Ϊ��λ���¶ȶ���
  * @param  out���з��ŵ�16λ�������¶����
  * @retval �����϶�Ϊ��λ�ĸ����¶�
*/
float M601_OutputtoTemp(int16_t out)
{
	return ((float)out/256.0 + 40.0);	
}

/**
  * @brief  �������϶�Ϊ��λ�ĸ����¶�ֵת��Ϊ16λ�����Ʋ����ʾ���¶�ֵ
  * @param  �����϶�Ϊ��λ�ĸ����¶�ֵ
  * @retval �з��ŵ�16λ�������¶�ֵ
*/
int16_t M601_TemptoOutput(float Temp)
{
	return (int16_t)((Temp-40.0)*256.0);	
}

/**
  * @brief  �������ֽ����е�У���
  * @param  serial���ֽ�����ָ��
  * @param  length���ֽ�����ĳ���
  * @retval У��ͣ�CRC��
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
  * @brief  ��оƬ�Ĵ������ݴ�����
  * @param  scr���ֽ�����ָ�룬 ����Ϊ @sizeof��M601_SCRATCHPAD_READ��
  * @retval ��״̬
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
  * @brief  доƬ�Ĵ������ݴ�����
  * @param  scr���ֽ�����ָ�룬 ����Ϊ @sizeof��M601_SCRATCHPAD_WRITE��
  * @retval д״̬
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
  * @brief  �����¶Ȳ���
  * @param  ��
  * @retval �����߷���״̬
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
  * @brief  �ȴ�ת��������������������@ConvertTemp����ʹ��
  * @param  iTemp�����ص�16λ�¶Ȳ������
  * @retval ��״̬
*/
BOOL ReadTempWaiting(uint16_t *iTemp)
{
	uint8_t scrb[sizeof(M601_SCRATCHPAD_READ)];
	M601_SCRATCHPAD_READ *scr = (M601_SCRATCHPAD_READ *) scrb;

	/*��9���ֽڡ�ǰ�������¶�ת�����������ֽ���ǰ8����У���--CRC��*/	
	if(M601_ReadScratchpad_SkipRom(scrb) == FALSE)
	{		
		return FALSE;  /*���Ĵ���ʧ��*/
	}

	/*������յ�ǰ8���ֽڵ�У��ͣ�������յĵ�9��CRC�ֽڱȽϡ�*/	

  if(scrb[8] != MY_OW_CRC8(scrb, 8))
  {	
		return FALSE;  /*CRC��֤δͨ��*/
  }

	/*���¶Ȳ�������������ֽںϳ�Ϊ16λ�֡�*/	
	*iTemp=(uint16_t)scr->T_msb<<8 | scr->T_lsb; 
	
  return TRUE;		
}

/**
  * @brief  ��ѯ�Ƿ�ת��������Ȼ��������������@ConvertTemp����ʹ��
  * @param  iTemp�����ص�16�¶Ȳ������
  * @retval �����״̬
*/
BOOL ReadTempPolling(uint16_t *iTemp)
{ int timeout = 0;
		
	/*��״̬λʱ϶�����ת����û������оƬ��1��Ӧ��ʱ϶�����ת��������оƬ��0��Ӧ��ʱ϶��
	ǰ�����ֽ����¶�ת�����������ֽ���ǰ8����У���--CRC��*/		
	while (OW_ReadStatus() == BUSY )
	{	
		DELAY_Ms(1);	
    timeout++; 
		if(timeout > 50) 
		{				
			return FALSE;				/*��ʱ����*/
		}
	}
	
	uint8_t scrb[sizeof(M601_SCRATCHPAD_READ)];
	M601_SCRATCHPAD_READ *scr = (M601_SCRATCHPAD_READ *) scrb;
	
	/*��9���ֽڡ�ǰ�������¶�ת�����������ֽ���ǰ8����У���--CRC��*/	
	if(M601_ReadScratchpad_SkipRom(scrb) == FALSE)
	{		
		return FALSE;  /*I2C��ַͷӦ��ΪNACK*/
	}

	/*������յ�ǰ8���ֽڵ�У��ͣ�������յĵ�9��CRC�ֽڱȽϡ�*/	
  if(scrb[8] != MY_OW_CRC8(scrb, 8))
  {	
		return FALSE;  /*CRC��֤δͨ��*/
  }
	/*���¶Ȳ�������������ֽںϳ�Ϊ16λ�֡�*/				
	*iTemp=(uint16_t)scr->T_msb<<8 | scr->T_lsb;
	
  return TRUE;		
}

/**
  * @brief  �������ڲ���Ƶ�ʺ��ظ���
  * @param  mps Ҫ���õ����ڲ���Ƶ�ʣ�ÿ�����������������Ϊ������һ
	*				@arg CFG_MPS_Single		��ÿִ��ConvertTempһ�Σ�����һ���¶Ȳ���
	*				@arg CFG_MPS_Half			��ÿִ��ConvertTempһ�Σ�����ÿ��0.5���ظ�����
	*				@arg CFG_MPS_1				��ÿִ��ConvertTempһ�Σ�����ÿ��1���ظ�����
	*				@arg CFG_MPS_2				��ÿִ��ConvertTempһ�Σ�����ÿ��2���ظ�����
	*				@arg CFG_MPS_4				��ÿִ��ConvertTempһ�Σ�����ÿ��4���ظ�����
	*				@arg CFG_MPS_10				��ÿִ��ConvertTempһ�Σ�����ÿ��10���ظ�����
  * @param  repeatability��Ҫ���õ��ظ���ֵ������Ϊ������һ
	*				@arg CFG_Repeatbility_Low				�����õ��ظ���
	*				@arg CFG_Repeatbility_Medium		���������ظ���
	*				@arg CFG_Repeatbility_High			�����ø��ظ���
  * @retval ��
*/
BOOL SetConfig(uint8_t mps, uint8_t repeatability)
{ 
	uint8_t scrb[sizeof(M601_SCRATCHPAD_READ)];
	M601_SCRATCHPAD_READ *scr = (M601_SCRATCHPAD_READ *) scrb;

	/*��9���ֽڡ���7�ֽ���ϵͳ���üĴ�������8�ֽ���ϵͳ״̬�Ĵ���������ֽ���ǰ8����У���--CRC��*/	
	if(M601_ReadScratchpad_SkipRom(scrb) == FALSE)
	{	
		return FALSE;  /*���ݴ�����ˮƽ*/
	}

	/*������յ�ǰ8���ֽڵ�У��ͣ�������յĵ�9��CRC�ֽڱȽϡ�*/	
  if(scrb[8] != MY_OW_CRC8(scrb, 8))
  {			
		return FALSE;  /*CRC��֤δͨ��*/
  }
	
	scr->Cfg &= ~CFG_Repeatbility_Mask;
	scr->Cfg |= repeatability;
	scr->Cfg &= ~CFG_MPS_Mask;
	scr->Cfg |= mps;		
	
	M601_WriteScratchpad_SkipRom(scrb+4);
	
	return TRUE;
}

/**
  * @brief  ��״̬������
  * @param  status ���ص�״̬�Ĵ���ֵ
  * @param  cfg ���ص����üĴ���ֵ
  * @retval ״̬
*/
BOOL ReadStatusConfig(uint8_t *status, uint8_t *cfg)
{ 
	uint8_t scrb[sizeof(M601_SCRATCHPAD_READ)];
	M601_SCRATCHPAD_READ *scr = (M601_SCRATCHPAD_READ *) scrb;

	/*��9���ֽڡ���7�ֽ���ϵͳ���üĴ�������8�ֽ���ϵͳ״̬�Ĵ���������ֽ���ǰ8����У���--CRC��*/		
	if(M601_ReadScratchpad_SkipRom(scrb) == FALSE)
	{
		return FALSE;  /*CRC��֤δͨ��*/
	}

	/*������յ�ǰ8���ֽڵ�У��ͣ�������յĵ�9��CRC�ֽڱȽϡ�*/	
  if(scrb[8] != MY_OW_CRC8(scrb, 8))
  {		
		return FALSE;  /*CRC��֤δͨ��*/
  }	
	
	*status = scr->Status;
	*cfg = scr->Cfg;	
	
	return TRUE;
}

/**
  * @brief  �����¶Ȳ���������
  * @param  ��
  * @retval ״̬
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
