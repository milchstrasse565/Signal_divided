#include "bsp_aht20.h"
#include "i2c.h"
#include "stdio.h"




/**************************************************
			��ȡAHT20��״̬�Ĵ���
**************************************************/
uint8_t AHT20_Read_Status(void)
{
	uint8_t Byte_first;	
//	IIC_Start();
//    IIC_Send_Byte(0x71);
//	flag=IIC_Wait_Ack();
//	Byte_first = IIC_Read_Byte(flag);
//	IIC_NAck();
//	IIC_Stop();
	
	HAL_I2C_Master_Receive(&hi2c2,0x70,&Byte_first,1,0xff);
	return Byte_first;
}



/**************************************************
				��AHT20����AC����
**************************************************/
void AHT20_SendAC(void) 
{
	uint8_t buffer[3] = {0xac,0x33,0x00};
//	IIC_Start();
//	IIC_Send_Byte(0x70);    //����������͵�01110000 �����1bit��ʾ��/д 0--д,1--����
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0xac);		//0xAC�ɼ����� ��������������ֽڣ���һ���ֽ�Ϊ 0x33���ڶ����ֽ�Ϊ0x00��
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x33);
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x00);
//	IIC_Wait_Ack();
//	IIC_Stop();


	while(HAL_I2C_Master_Transmit(&hi2c2,0x70,buffer,3,0xff) != HAL_OK);
}



/**************************************************
				��ʼ��AHT20
**************************************************/
void AHT20_Init(void)   
{	
	uint8_t buffer1[3] = {0xa8,0x00,0x00};
	uint8_t buffer2[3] = {0xbe,0x08,0x00};
//	IIC_Init();
//	IIC_Start();
//	IIC_Send_Byte(0x70);
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0xa8);//0xA8����NOR����ģʽ
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x00);
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x00);
//	IIC_Wait_Ack();
//	IIC_Stop();

	HAL_Delay(10);//��ʱ10ms����
	while(HAL_I2C_Master_Transmit(&hi2c2,0x70,buffer1,3,0xff) != HAL_OK);
	

//	IIC_Start();
//	IIC_Send_Byte(0x70);
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0xbe);//0xBE��ʼ�����AHT20�ĳ�ʼ��������0xBE,   AHT10�ĳ�ʼ��������0xE1
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x08);//��ؼĴ���bit[3]��1��ΪУ׼���
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x00);
//	IIC_Wait_Ack();
//	IIC_Stop();
	while(HAL_I2C_Master_Transmit(&hi2c2,0x70,buffer2,3,0xff) != HAL_OK);
	HAL_Delay(10);//��ʱ10ms����
}


  
/**************************************************
	û��CRCУ�飬ֱ�Ӷ�ȡAHT20���¶Ⱥ�ʪ������ 
**************************************************/
void AHT20_Read_CTdata(uint32_t *ct) 
{
	volatile uint8_t Byte_1th=0,Byte_2th=0,Byte_3th=0;
    volatile uint8_t Byte_4th=0,Byte_5th=0,Byte_6th=0;
	uint8_t buffer[6] = {0};
	uint32_t RetuData = 0;
	uint16_t cnt = 0;
	AHT20_SendAC();//��AHT20����AC����
	HAL_Delay(80);	//��Լ��ʱ80ms
    
	while(((AHT20_Read_Status()&0x80)==0x80))//ֱ��״̬bit[7]Ϊ0����ʾΪ����״̬����Ϊ1����ʾæ״̬
	{
		HAL_Delay(1);
		if(cnt++>=100) break;
	}
    
//	IIC_Start();
//	IIC_Send_Byte(0x71);
//	flag=IIC_Wait_Ack();
//	Byte_1th = IIC_Read_Byte(flag);//״̬��
//	Byte_2th = IIC_Read_Byte(flag);//ʪ��,����ACK(��������)
//	Byte_3th = IIC_Read_Byte(flag);//ʪ��
//	Byte_4th = IIC_Read_Byte(flag);//ʪ��/�¶�
//	Byte_5th = IIC_Read_Byte(flag);//�¶�
//	Byte_6th = IIC_Read_Byte(!flag);//�¶�,����NACK(ֹͣ����)  
//	IIC_Stop();
	
	HAL_I2C_Master_Receive(&hi2c2,0x70,buffer,6,0xff);
	Byte_1th = buffer[0];
	Byte_2th = buffer[1];
	Byte_3th = buffer[2];
	Byte_4th = buffer[3];
	Byte_5th = buffer[4];
	Byte_6th = buffer[5];
	
	
    
    //����õ������ݵ�RetuData��
	RetuData = (RetuData|Byte_2th)<<8;  
	RetuData = (RetuData|Byte_3th)<<8;
	RetuData = (RetuData|Byte_4th);
	RetuData =RetuData >>4;
	ct[0] = RetuData;//ʪ��
    
	RetuData = 0;
	RetuData = (RetuData|Byte_4th)<<8;
	RetuData = (RetuData|Byte_5th)<<8;
	RetuData = (RetuData|Byte_6th);
	RetuData = RetuData&0x0fffff;
	ct[1] =RetuData; //�¶�
}



/**************************************************
			��ʼ����ʪ�ȴ�����
**************************************************/
void temphum_init()
{
	HAL_Delay(40);//���ϵ磬��ʱ40ms�ſ��Զ�ȡ״̬
	
    //���ȷ�0x71��ȡ״̬��bit[3]�����=1,ΪУ׼����������ʼ��!!!��������¶�������״̬��0x1C������0x18,��������0x80��ʾæ״̬;
	if(!((AHT20_Read_Status()&0x08)==0x08))
	{
		printf("��ʼ����ʪ��\r\n");
    AHT20_Init(); //��ʼ��AHT20 
		HAL_Delay(100);
	}
}


float aht20_GetTemp(uint32_t *ct)
{
	float Temp;
	Temp = ct[0] / 1048576.0;
	return Temp;
}

float aht20_GetHum(uint32_t *ct)
{
	float Hum;
	Hum = (ct[1] / 1048576.0) * 200.0 - 50.0;
	return Hum;
}

