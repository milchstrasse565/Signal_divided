#include "IMU_APP.h"
#include "MPU6050.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

#include "stdio.h"

uint32_t LastPrintfTime = 0;

void IMU_Task_Main(void const * argument)
{
//	MPU6050_Init ();
	osDelay(100);																	//�ȴ�M601��ʼ��
	printf("6050Finish\r\n");
	while(1)
	{
		//if(MPU_Data_Ready == 1)										//�ȴ������ǲ������
		//{
			MPU_Data_Ready = 0;												//����ź���
			
			MPU6050_Read_Accel ();										//�����ٶȼ�
			MPU6050_Read_Gyro  ();										//��������
			MPU6050_Read_Temp  ();										//���¶�
		//}
		
		if(HAL_GetTick() - LastPrintfTime > 100)		//ÿһ��ʱ�䣬��ӡһ������
		{
			LastPrintfTime = HAL_GetTick();
			ShowResult();															//ͨ�����ڴ�ӡ���
			//ShowResult_FireWater();									//ͨ�����ڻ�ͼ
		}
		osDelay(10);
	}
}
