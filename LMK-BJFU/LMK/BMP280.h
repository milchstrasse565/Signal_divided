#ifndef __BMP280_H
#define __BMP280_H

#include "main.h"

//#include "sys.h"
//#include "stdbool.h"
#include <math.h>
//#include "delay.h"
//#include "usart.h"

#define BMP_FALSE   0
#define BMP_TRUE   	1


#define BMP280_ADDR									(0xEC)
#define BMP280_DEFAULT_CHIP_ID			(0x58)

#define BMP280_CHIP_ID							(0xD0)         /* Chip ID Register */
#define BMP280_RST_REG							(0xE0)         /* Softreset Register */
#define BMP280_STAT_REG							(0xF3)         /* Status Register */
#define BMP280_CTRL_MEAS_REG				(0xF4)         /* Ctrl Measure Register */
#define BMP280_CONFIG_REG						(0xF5)         /* Configuration Register */
#define BMP280_PRESSURE_MSB_REG			(0xF7)         /* Pressure MSB Register */
#define BMP280_PRESSURE_LSB_REG			(0xF8)         /* Pressure LSB Register */
#define BMP280_PRESSURE_XLSB_REG		(0xF9)         /* Pressure XLSB Register */
#define BMP280_TEMPERATURE_MSB_REG	(0xFA)         /* Temperature MSB Reg */
#define BMP280_TEMPERATURE_LSB_REG	(0xFB)         /* Temperature LSB Reg */
#define BMP280_TEMPERATURE_XLSB_REG	(0xFC)         /* Temperature XLSB Reg */

#define BMP280_SLEEP_MODE						(0x00)
#define BMP280_FORCED_MODE					(0x01)
#define BMP280_NORMAL_MODE					(0x03)

#define BMP280_TEMPERATURE_CALIB_DIG_T1_LSB_REG             (0x88)
#define BMP280_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH       (24)
#define BMP280_DATA_FRAME_SIZE			(6)

#define BMP280_OVERSAMP_SKIPPED			(0x00)
#define BMP280_OVERSAMP_1X					(0x01)
#define BMP280_OVERSAMP_2X					(0x02)
#define BMP280_OVERSAMP_4X					(0x03)
#define BMP280_OVERSAMP_8X					(0x04)
#define BMP280_OVERSAMP_16X					(0x05)


uint8_t bmp280Init(void);
void bmp280GetData(float* pressure, float* temperature, float* asl);


//IO��������
#define SDA_IN()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;}
#define SDA_OUT() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;}

//IO��������	 
#define IIC_SCL    PCout(12)                                                   	//SCL
#define IIC_SDA    PCout(11)                                                   	//SDA	 
#define READ_SDA   PCin(11)                                                    	//����SDA 

//IIC���в�������
void BMP_IIC_Init(void);                                                       	//��ʼ��IIC��IO��				 
void BMP_IIC_Start(void);				                                           			//����IIC��ʼ�ź�
void BMP_IIC_Stop(void);	  			                                           		//����IICֹͣ�ź�
void BMP_IIC_Send_Byte(uint8_t txd);			                                      //IIC����һ���ֽ�
uint8_t   BMP_IIC_Read_Byte(unsigned char ack);                                 //IIC��ȡһ���ֽ�
uint8_t   BMP_IIC_Wait_Ack(void); 				                                      //IIC�ȴ�ACK�ź�
void BMP_IIC_Ack(void);					                                           			//IIC����ACK�ź�
void BMP_IIC_NAck(void);				                                           			//IIC������ACK�ź�

void BMP_IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t   BMP_IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	
			 
uint8_t   BMP_iicDevReadByte(uint8_t devaddr,uint8_t addr);	                    /*��һ�ֽ�*/
void BMP_iicDevWriteByte(uint8_t devaddr,uint8_t addr,uint8_t data);	          /*дһ�ֽ�*/
void BMP_iicDevRead(uint8_t devaddr,uint8_t addr,uint8_t len,uint8_t *rbuf);    /*������ȡ����ֽ�*/
void BMP_iicDevWrite(uint8_t devaddr,uint8_t addr,uint8_t len,uint8_t *wbuf);   /*����д�����ֽ�*/

uint8_t bmp280Init(void);	//��ʼ��

#endif
