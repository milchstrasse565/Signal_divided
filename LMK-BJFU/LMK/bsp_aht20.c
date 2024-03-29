#include "bsp_aht20.h"
#include "i2c.h"
#include "stdio.h"




/**************************************************
			读取AHT20的状态寄存器
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
				向AHT20发送AC命令
**************************************************/
void AHT20_SendAC(void) 
{
	uint8_t buffer[3] = {0xac,0x33,0x00};
//	IIC_Start();
//	IIC_Send_Byte(0x70);    //启动传输后发送的01110000 （最后1bit表示读/写 0--写,1--读）
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0xac);		//0xAC采集命令 命令参数有两个字节，第一个字节为 0x33，第二个字节为0x00。
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x33);
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x00);
//	IIC_Wait_Ack();
//	IIC_Stop();


	while(HAL_I2C_Master_Transmit(&hi2c2,0x70,buffer,3,0xff) != HAL_OK);
}



/**************************************************
				初始化AHT20
**************************************************/
void AHT20_Init(void)   
{	
	uint8_t buffer1[3] = {0xa8,0x00,0x00};
	uint8_t buffer2[3] = {0xbe,0x08,0x00};
//	IIC_Init();
//	IIC_Start();
//	IIC_Send_Byte(0x70);
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0xa8);//0xA8进入NOR工作模式
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x00);
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x00);
//	IIC_Wait_Ack();
//	IIC_Stop();

	HAL_Delay(10);//延时10ms左右
	while(HAL_I2C_Master_Transmit(&hi2c2,0x70,buffer1,3,0xff) != HAL_OK);
	

//	IIC_Start();
//	IIC_Send_Byte(0x70);
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0xbe);//0xBE初始化命令，AHT20的初始化命令是0xBE,   AHT10的初始化命令是0xE1
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x08);//相关寄存器bit[3]置1，为校准输出
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x00);
//	IIC_Wait_Ack();
//	IIC_Stop();
	while(HAL_I2C_Master_Transmit(&hi2c2,0x70,buffer2,3,0xff) != HAL_OK);
	HAL_Delay(10);//延时10ms左右
}


  
/**************************************************
	没有CRC校验，直接读取AHT20的温度和湿度数据 
**************************************************/
void AHT20_Read_CTdata(uint32_t *ct) 
{
	volatile uint8_t Byte_1th=0,Byte_2th=0,Byte_3th=0;
    volatile uint8_t Byte_4th=0,Byte_5th=0,Byte_6th=0;
	uint8_t buffer[6] = {0};
	uint32_t RetuData = 0;
	uint16_t cnt = 0;
	AHT20_SendAC();//向AHT20发送AC命令
	HAL_Delay(80);	//大约延时80ms
    
	while(((AHT20_Read_Status()&0x80)==0x80))//直到状态bit[7]为0，表示为空闲状态，若为1，表示忙状态
	{
		HAL_Delay(1);
		if(cnt++>=100) break;
	}
    
//	IIC_Start();
//	IIC_Send_Byte(0x71);
//	flag=IIC_Wait_Ack();
//	Byte_1th = IIC_Read_Byte(flag);//状态字
//	Byte_2th = IIC_Read_Byte(flag);//湿度,发送ACK(继续发送)
//	Byte_3th = IIC_Read_Byte(flag);//湿度
//	Byte_4th = IIC_Read_Byte(flag);//湿度/温度
//	Byte_5th = IIC_Read_Byte(flag);//温度
//	Byte_6th = IIC_Read_Byte(!flag);//温度,发送NACK(停止发送)  
//	IIC_Stop();
	
	HAL_I2C_Master_Receive(&hi2c2,0x70,buffer,6,0xff);
	Byte_1th = buffer[0];
	Byte_2th = buffer[1];
	Byte_3th = buffer[2];
	Byte_4th = buffer[3];
	Byte_5th = buffer[4];
	Byte_6th = buffer[5];
	
	
    
    //保存得到的数据到RetuData中
	RetuData = (RetuData|Byte_2th)<<8;  
	RetuData = (RetuData|Byte_3th)<<8;
	RetuData = (RetuData|Byte_4th);
	RetuData =RetuData >>4;
	ct[0] = RetuData;//湿度
    
	RetuData = 0;
	RetuData = (RetuData|Byte_4th)<<8;
	RetuData = (RetuData|Byte_5th)<<8;
	RetuData = (RetuData|Byte_6th);
	RetuData = RetuData&0x0fffff;
	ct[1] =RetuData; //温度
}



/**************************************************
			初始化温湿度传感器
**************************************************/
void temphum_init()
{
	HAL_Delay(40);//刚上电，延时40ms才可以读取状态
	
    //首先发0x71读取状态字bit[3]，如果=1,为校准输出，无须初始化!!!正常情况下读回来的状态是0x1C或者是0x18,读回来是0x80表示忙状态;
	if(!((AHT20_Read_Status()&0x08)==0x08))
	{
		printf("初始化温湿度\r\n");
    AHT20_Init(); //初始化AHT20 
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

