#include "bmp280.h"
#include "i2c.h"
#include "stdio.h"
//��ʼ��IIC
void BMP_IIC_Init(void)
{					     
//	GPIO_InitTypeDef GPIO_InitStructure;
//	//RCC->APB2ENR|=1<<4;//��ʹ������IO PORTCʱ�� 
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );	
//	   
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_11;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP ;                            //�������
//	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC,&GPIO_InitStructure);
// 
//	IIC_SCL=1;
//	IIC_SDA=1;

}
////����IIC��ʼ�ź�
//void BMP_IIC_Start(void)
//{
//	SDA_OUT();                                                                 //sda�����
//	IIC_SDA=1;	  	  
//	IIC_SCL=1;
//	delay_us(4);
// 	IIC_SDA=0;                                                                 //START:when CLK is high,DATA change form high to low 
//	delay_us(4);
//	IIC_SCL=0;                                                                 //ǯסI2C���ߣ�׼�����ͻ�������� 
//}	  
////����IICֹͣ�ź�
//void BMP_IIC_Stop(void)
//{
//	SDA_OUT();                                                                 //sda�����
//	IIC_SCL=0;
//	IIC_SDA=0;                                                                 //STOP:when CLK is high DATA change form low to high
// 	delay_us(4);
//	IIC_SCL=1; 
//	IIC_SDA=1;                                                                 //����I2C���߽����ź�
//	delay_us(4);							   	
//}
////�ȴ�Ӧ���źŵ���
////����ֵ��1������Ӧ��ʧ��
////        0������Ӧ��ɹ�
//u8 BMP_IIC_Wait_Ack(void)
//{
//	u8 ucErrTime=0;
//	SDA_IN();                                                                  //SDA����Ϊ����  
//	IIC_SDA=1;delay_us(1);	   
//	IIC_SCL=1;delay_us(1);	 
//	while(READ_SDA)
//	{
//		ucErrTime++;
//		if(ucErrTime>250)
//		{
//			BMP_IIC_Stop();
//			return 1;
//		}
//	}
//	IIC_SCL=0;                                                                 //ʱ�����0 	   
//	return 0;  
//} 
////����ACKӦ��
//void BMP_IIC_Ack(void)
//{
//	IIC_SCL=0;
//	SDA_OUT();
//	IIC_SDA=0;
//	delay_us(2);
//	IIC_SCL=1;
//	delay_us(2);
//	IIC_SCL=0;
//}
////������ACKӦ��		    
//void BMP_IIC_NAck(void)
//{
//	IIC_SCL=0;
//	SDA_OUT();
//	IIC_SDA=1;
//	delay_us(2);
//	IIC_SCL=1;
//	delay_us(2);
//	IIC_SCL=0;
//}					 				     
////IIC����һ���ֽ�
////���شӻ�����Ӧ��
////1����Ӧ��
////0����Ӧ��			  
//void BMP_IIC_Send_Byte(u8 txd)
//{                        
//    u8 t;   
//	SDA_OUT(); 	    
//    IIC_SCL=0;                                                                 //����ʱ�ӿ�ʼ���ݴ���
//    for(t=0;t<8;t++)
//    {              
//        IIC_SDA=(txd&0x80)>>7;
//        txd<<=1; 	  
//		delay_us(2);                                                           //��TEA5767��������ʱ���Ǳ����
//		IIC_SCL=1;
//		delay_us(2); 
//		IIC_SCL=0;	
//		delay_us(2);
//    }	 
//} 	    
////��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
//u8 BMP_IIC_Read_Byte(unsigned char ack)
//{
//	unsigned char i,receive=0;
//	SDA_IN();                                                                  //SDA����Ϊ����
//    for(i=0;i<8;i++ )
//	{
//        IIC_SCL=0; 
//        delay_us(2);
//		IIC_SCL=1;
//        receive<<=1;
//        if(READ_SDA)receive++;   
//		delay_us(1); 
//    }					 
//    if (!ack)
//        BMP_IIC_NAck();                                                            //����nACK
//    else
//        BMP_IIC_Ack();                                                             //����ACK   
//    return receive;
//}

//��ָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
uint8_t BMP_iicDevReadByte(uint8_t devaddr,uint8_t addr)
{				  
	uint8_t temp=0;		  	    																 
//	BMP_IIC_Start();  
//	BMP_IIC_Send_Byte(devaddr);                                                    //��������д���� 	   
//	BMP_IIC_Wait_Ack(); 
//	BMP_IIC_Send_Byte(addr);                                                       //���͵͵�ַ
//	BMP_IIC_Wait_Ack();	

//	BMP_IIC_Start();  	 	   
//	BMP_IIC_Send_Byte(devaddr|1);                                                  //��������������			   
//	BMP_IIC_Wait_Ack();	 
//	temp=BMP_IIC_Read_Byte(0);			   
//	BMP_IIC_Stop();  
                                                              //����һ��ֹͣ����	    
	
	HAL_I2C_Master_Receive(&hi2c2,devaddr,&temp,1,0xff);
	return temp;
}

//����������ֽ�
//addr:��ʼ��ַ
//rbuf:�����ݻ���
//len:���ݳ���
void BMP_iicDevRead(uint8_t devaddr,uint8_t addr,uint8_t len,uint8_t *rbuf)
{
	HAL_I2C_Mem_Read(&hi2c2,devaddr,addr,1,rbuf,len,0xff);
//	int i=0;
//	BMP_IIC_Start();  
//	BMP_IIC_Send_Byte(devaddr);  
//	BMP_IIC_Wait_Ack();	
//	BMP_IIC_Send_Byte(addr);                                                       //��ַ����  
//	BMP_IIC_Wait_Ack();	

//	BMP_IIC_Start();  	
//	BMP_IIC_Send_Byte(devaddr|1);  	
//	BMP_IIC_Wait_Ack();	
//	for(i=0; i<len; i++)
//	{
//		if(i==len-1)
//		{
//			rbuf[i]=BMP_IIC_Read_Byte(0);                                          //���һ���ֽڲ�Ӧ��
//		}
//		else
//			rbuf[i]=BMP_IIC_Read_Byte(1);
//	}
//	BMP_IIC_Stop( );	
}

//��ָ����ַд��һ������
//WriteAddr :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void BMP_iicDevWriteByte(uint8_t devaddr,uint8_t addr,uint8_t data)
{		
	HAL_I2C_Mem_Write(&hi2c2,devaddr,addr,1,&data,1,0xff);
//	BMP_IIC_Start();  
//	BMP_IIC_Send_Byte(devaddr);                                                    //��������д���� 	 
//	BMP_IIC_Wait_Ack();	   
//	BMP_IIC_Send_Byte(addr);                                                       //���͵͵�ַ
//	BMP_IIC_Wait_Ack(); 	 										  		   
//	BMP_IIC_Send_Byte(data);                                                       //�����ֽ�							   
//	BMP_IIC_Wait_Ack();  		    	   
//	BMP_IIC_Stop();		                                                           //����һ��ֹͣ���� 	 
}

//����д����ֽ�
//addr:��ʼ��ַ
//wbuf:д���ݻ���
//len:���ݵĳ���
void BMP_iicDevWrite(uint8_t devaddr,uint8_t addr,uint8_t len,uint8_t *wbuf)
{
//	int i=0;
//	BMP_IIC_Start();  
//	BMP_IIC_Send_Byte(devaddr);  	
//	BMP_IIC_Wait_Ack();	
//	BMP_IIC_Send_Byte(addr);  //��ַ����
//	BMP_IIC_Wait_Ack();	
//	for(i=0; i<len; i++)
//	{
//		BMP_IIC_Send_Byte(wbuf[i]);  
//		BMP_IIC_Wait_Ack();		
//	}
//	BMP_IIC_Stop( );	
	HAL_I2C_Mem_Write(&hi2c2,devaddr,addr,1,wbuf,len,0xff);
}



/*bmp280 ��ѹ���¶ȹ����� ����ģʽ*/
#define BMP280_PRESSURE_OSR			(BMP280_OVERSAMP_8X)
#define BMP280_TEMPERATURE_OSR		(BMP280_OVERSAMP_16X)
#define BMP280_MODE					(BMP280_PRESSURE_OSR<<2|BMP280_TEMPERATURE_OSR<<5|BMP280_NORMAL_MODE)

typedef signed char s8;  
typedef unsigned char u8;  
  
typedef signed short s16;  
typedef unsigned short u16;  
  
typedef signed int s32;  
typedef unsigned int u32;  
  
typedef signed long long s64;  
typedef unsigned long long u64;  



typedef struct 
{
    uint16_t dig_T1;                                             /* calibration T1 data */
    s16 dig_T2;                                             /* calibration T2 data */
    s16 dig_T3;                                             /* calibration T3 data */
    u16 dig_P1;                                             /* calibration P1 data */
    s16 dig_P2;                                             /* calibration P2 data */
    s16 dig_P3;                                             /* calibration P3 data */
    s16 dig_P4;                                             /* calibration P4 data */
    s16 dig_P5;                                             /* calibration P5 data */
    s16 dig_P6;                                             /* calibration P6 data */
    s16 dig_P7;                                             /* calibration P7 data */
    s16 dig_P8;                                             /* calibration P8 data */
    s16 dig_P9;                                             /* calibration P9 data */
    s32 t_fine;                                             /* calibration t_fine data */
} bmp280Calib;

bmp280Calib  bmp280Cal;

static uint8_t bmp280ID=0;
static uint8_t isInit = BMP_FALSE;
static s32 bmp280RawPressure=0;
static s32 bmp280RawTemperature=0;

static void bmp280GetPressure(void);
static void presssureFilter(float* in,float* out);
static float bmp280PressureToAltitude(float* pressure/*, float* groundPressure, float* groundTemp*/);

uint8_t bmp280Init(void)
{	
    if (isInit)
        return BMP_TRUE;

	BMP_IIC_Init();		                                                           /*��ʼ��I2C*/
    HAL_Delay(20);
	
	bmp280ID=BMP_iicDevReadByte(BMP280_ADDR,BMP280_CHIP_ID);	                   /* ��ȡbmp280 ID*/
	
	if(bmp280ID==BMP280_DEFAULT_CHIP_ID)
		printf("BMP280 ID IS: 0x%X\n",bmp280ID);
    else
        return BMP_FALSE;

    /* ��ȡУ׼���� */
    BMP_iicDevRead(BMP280_ADDR,BMP280_TEMPERATURE_CALIB_DIG_T1_LSB_REG,24,(uint8_t *)&bmp280Cal);	
	BMP_iicDevWriteByte(BMP280_ADDR,BMP280_CTRL_MEAS_REG,BMP280_MODE);
	BMP_iicDevWriteByte(BMP280_ADDR,BMP280_CONFIG_REG,5<<2);		               /*����IIR�˲�*/
	
//	printf("BMP280 Calibrate Registor Are: \r\n");
//	for(i=0;i<24;i++)
//		printf("Registor %2d: 0x%X\n",i,p[i]);
	
    isInit = BMP_TRUE;
    return BMP_TRUE;
}

static void bmp280GetPressure(void)
{
    uint8_t data[BMP280_DATA_FRAME_SIZE];

    // read data from sensor
    BMP_iicDevRead(BMP280_ADDR,BMP280_PRESSURE_MSB_REG,BMP280_DATA_FRAME_SIZE,data);
    bmp280RawPressure=(s32)((((uint32_t)(data[0]))<<12)|(((uint32_t)(data[1]))<<4)|((uint32_t)data[2]>>4));
    bmp280RawTemperature=(s32)((((uint32_t)(data[3]))<<12)|(((uint32_t)(data[4]))<<4)|((uint32_t)data[5]>>4));
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of "5123" equals 51.23 DegC
// t_fine carries fine temperature as global value
static s32 bmp280CompensateT(s32 adcT)
{
    s32 var1,var2,T;

    var1=((((adcT>>3)-((s32)bmp280Cal.dig_T1<<1)))*((s32)bmp280Cal.dig_T2))>>11;
    var2=(((((adcT>>4)-((s32)bmp280Cal.dig_T1))*((adcT>>4)-((s32)bmp280Cal.dig_T1)))>>12)*((s32)bmp280Cal.dig_T3))>>14;
    bmp280Cal.t_fine=var1+var2;
	
    T=(bmp280Cal.t_fine*5+128)>>8;

    return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of "24674867" represents 24674867/256 = 96386.2 Pa = 963.862 hPa
static uint32_t bmp280CompensateP(s32 adcP)
{
    int64_t var1,var2,p;
    var1=((int64_t)bmp280Cal.t_fine)-128000;
    var2=var1*var1*(int64_t)bmp280Cal.dig_P6;
    var2=var2+((var1*(int64_t)bmp280Cal.dig_P5)<<17);
    var2=var2+(((int64_t)bmp280Cal.dig_P4)<<35);
    var1=((var1*var1*(int64_t)bmp280Cal.dig_P3)>>8)+((var1*(int64_t)bmp280Cal.dig_P2)<<12);
    var1=(((((int64_t)1)<<47)+var1))*((int64_t)bmp280Cal.dig_P1)>>33;
    if (var1==0)
        return 0;
    p=1048576-adcP;
    p=(((p<<31)-var2)*3125)/var1;
    var1=(((int64_t)bmp280Cal.dig_P9)*(p>>13)*(p>>13))>>25;
    var2=(((int64_t)bmp280Cal.dig_P8)*p)>>19;
    p=((p+var1+var2)>>8)+(((int64_t)bmp280Cal.dig_P7)<<4);
    return(uint32_t)p;
}

#define FILTER_NUM	5
#define FILTER_A	0.1f

/*�޷�ƽ���˲���*/
static void presssureFilter(float* in,float* out)
{	
	static uint8_t i=0;
	static float filter_buf[FILTER_NUM]={0.0};
	double filter_sum=0.0;
	uint8_t cnt=0;	
	float deta;

	if(filter_buf[i]==0.0f)
	{
		filter_buf[i]=*in;
		*out=*in;
		if(++i>=FILTER_NUM)	
			i=0;
	} 
	else 
	{
		if(i)
			deta=*in-filter_buf[i-1];
		else 
			deta=*in-filter_buf[FILTER_NUM-1];
		
		if(fabs(deta)<FILTER_A)
		{
			filter_buf[i]=*in;
			if(++i>=FILTER_NUM)	
				i=0;
		}
		for(cnt=0;cnt<FILTER_NUM;cnt++)
		{
			filter_sum+=filter_buf[cnt];
		}
		*out=filter_sum /FILTER_NUM;
	}
}

void bmp280GetData(float* pressure,float* temperature,float* asl)
{
    static float t;
    static float p;
	
	bmp280GetPressure();

	t=bmp280CompensateT(bmp280RawTemperature)/100.0;		
	p=bmp280CompensateP(bmp280RawPressure)/25600.0;		

	presssureFilter(&p,pressure);
	*temperature=(float)t;                                                     /*��λ��*/
//	*pressure=(float)p ;	                                                   /*��λhPa*/	
	
	*asl=bmp280PressureToAltitude(pressure);	                               /*ת���ɺ���*/	
}

#define CONST_PF 0.1902630958	                                               //(1/5.25588f) Pressure factor
#define FIX_TEMP 25				                                               // Fixed Temperature. ASL is a function of pressure and temperature, but as the temperature changes so much (blow a little towards the flie and watch it drop 5 degrees) it corrupts the ASL estimates.
								                                               // TLDR: Adjusting for temp changes does more harm than good.
/*
 * Converts pressure to altitude above sea level (ASL) in meters
*/
static float bmp280PressureToAltitude(float* pressure/*, float* groundPressure, float* groundTemp*/)
{
    if (*pressure>0)
    {
        return((pow((1015.7f/ *pressure),CONST_PF)-1.0f)*(FIX_TEMP+273.15f))/0.0065f;
    }
    else
    {
        return 0;
    }
}
