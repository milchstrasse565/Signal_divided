#ifndef __MPU6050_H
#define __MPU6050_H

#include "main.h"


#define MPU6050_ADDR 0xD0


#define SMPLRT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1c
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_XOUT_H_REG 0x43
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75


extern int16_t Accel_X_RAW;
extern int16_t Accel_Y_RAW;
extern int16_t Accel_Z_RAW;
extern int16_t Gyro_X_RAW;
extern int16_t Gyro_Y_RAW;
extern int16_t Gyro_Z_RAW;

extern int16_t Temp_RAW;

extern float Ax,Ay,Az,Gx,Gy,Gz,Temp;

extern uint8_t MPU_Data_Ready;


void MPU6050_Init(void );									//初始化

void MPU6050_Read_Accel(void);						//读角加速度
void MPU6050_Read_Gyro(void );						//读线加速度
void MPU6050_Read_Temp(void );						//读温度

void ShowResult(void );										//通过printf展示结果
void ShowResult_FireWater(void);					//绘图

void MPU_6050_IRQ_Handle(void);						//中断回调函数

#endif      //__MPU6050_H



