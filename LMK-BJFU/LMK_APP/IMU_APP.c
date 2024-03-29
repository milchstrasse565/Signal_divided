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
	osDelay(100);																	//等待M601初始化
	printf("6050Finish\r\n");
	while(1)
	{
		//if(MPU_Data_Ready == 1)										//等待陀螺仪采样完成
		//{
			MPU_Data_Ready = 0;												//清空信号量
			
			MPU6050_Read_Accel ();										//读加速度计
			MPU6050_Read_Gyro  ();										//读陀螺仪
			MPU6050_Read_Temp  ();										//读温度
		//}
		
		if(HAL_GetTick() - LastPrintfTime > 100)		//每一定时间，打印一次数据
		{
			LastPrintfTime = HAL_GetTick();
			ShowResult();															//通过串口打印结果
			//ShowResult_FireWater();									//通过串口绘图
		}
		osDelay(10);
	}
}
