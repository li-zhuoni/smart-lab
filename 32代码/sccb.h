#ifndef __SCCB_H
#define __SCCB_H
#include "sys.h"
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//OVϵ������ͷ SCCB ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/14
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 


//ע��SCCB��SDA��SCL���ű����ǿ�©���

//IO��������
#define SCCB_SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9 ����
#define SCCB_SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} 	//PB9 ���


//IO��������	 
#define SCCB_SCL(x)    	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,x)	//PDout(6)	 	//SCL
#define SCCB_SDA(x)    	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,x)	//PDout(7) 		//SDA	 

#define SCCB_READ_SDA   HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9) 	//PDin(7)  		//����SDA    
#define SCCB_ID   			0X60  			//OV2640��ID     The device slave address are 60 for write and 61 for read.

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

