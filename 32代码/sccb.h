#ifndef __SCCB_H
#define __SCCB_H
#include "sys.h"
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//OV系列摄像头 SCCB 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/14
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 


//注意SCCB的SDA和SCL引脚必须是开漏输出

//IO方向设置
#define SCCB_SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9 输入
#define SCCB_SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} 	//PB9 输出


//IO操作函数	 
#define SCCB_SCL(x)    	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,x)	//PDout(6)	 	//SCL
#define SCCB_SDA(x)    	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,x)	//PDout(7) 		//SDA	 

#define SCCB_READ_SDA   HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9) 	//PDin(7)  		//输入SDA    
#define SCCB_ID   			0X60  			//OV2640的ID     The device slave address are 60 for write and 61 for read.

///////////////////////////////////////////
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
uint8_t SCCB_WR_Byte(uint8_t dat);
uint8_t SCCB_RD_Byte(void);
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data);
uint8_t SCCB_RD_Reg(uint8_t reg);
#endif

