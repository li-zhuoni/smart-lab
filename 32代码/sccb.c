#include "sys.h"
#include "sccb.h"
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

//初始化SCCB接口 
void SCCB_Init(void)
{				
  /*
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOD时钟
	
  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;//PD6,7 推挽输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //PD6,7 推挽输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
 
	GPIO_SetBits(GPIOD,GPIO_Pin_6|GPIO_Pin_7);
	SCCB_SDA_OUT();	   
	*/
	
	GPIO_InitTypeDef GPIO_InitStructure;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStructure.Pin = GPIO_PIN_10|GPIO_PIN_11;//PDB10,11 推挽输出
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;  //PB10,11 推挽输出
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;//100MHz
  GPIO_InitStructure.Pull = GPIO_PULLUP;//上拉
  HAL_GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10|GPIO_PIN_1,1);
	SCCB_SDA_OUT();	
}			 

//SCCB起始信号
//当时钟为高的时候,数据线的高到低,为SCCB起始信号
//在激活状态下,SDA和SCL均为低电平
void SCCB_Start(void)
{
    SCCB_SDA(1);     //数据线高电平	   
    SCCB_SCL(1);	    //在时钟线高的时候数据线由高至低
    HAL_Delay(1);  
    SCCB_SDA(0);
    HAL_Delay(1);	 
    SCCB_SCL(0);	    //数据线恢复低电平，单操作函数必要	  
}

//SCCB停止信号
//当时钟为高的时候,数据线的低到高,为SCCB停止信号
//空闲状况下,SDA,SCL均为高电平
void SCCB_Stop(void)
{
    SCCB_SDA(0);
    HAL_Delay(1);	 
    SCCB_SCL(1);	
    HAL_Delay(1); 
    SCCB_SDA(1);	
    HAL_Delay(1);
}  
//产生NA信号
void SCCB_No_Ack(void)
{
	HAL_Delay(1);
	SCCB_SDA(1);	
	SCCB_SCL(1);	
	HAL_Delay(1);
	SCCB_SCL(0);	
	HAL_Delay(1);
	SCCB_SDA(0);	
	HAL_Delay(1);
}
//SCCB,写入一个字节
//返回值:0,成功;1,失败. 
uint8_t SCCB_WR_Byte(uint8_t dat)
{
	uint8_t j,res;	 
	for(j=0;j<8;j++) //循环8次发送数据
	{
		if(dat&0x80)SCCB_SDA(1);	
		else SCCB_SDA(0);
		dat<<=1;
		HAL_Delay(1);
		SCCB_SCL(1);	
		HAL_Delay(1);
		SCCB_SCL(0);		   
	}			 
	SCCB_SDA_IN();		//设置SDA为输入 
	HAL_Delay(1);
	SCCB_SCL(1);			//接收第九位,以判断是否发送成功
	HAL_Delay(1);
	if(SCCB_READ_SDA)res=1;  //SDA=1发送失败，返回1
	else res=0;         //SDA=0发送成功，返回0
	SCCB_SCL(0);		 
	SCCB_SDA_OUT();		//设置SDA为输出    
	return res;  
}	 
//SCCB 读取一个字节
//在SCL的上升沿,数据锁存
//返回值:读到的数据
uint8_t SCCB_RD_Byte(void)
{
	uint8_t temp=0,j;    
	SCCB_SDA_IN();		//设置SDA为输入  
	for(j=8;j>0;j--) 	//循环8次接收数据
	{		     	  
		HAL_Delay(1);
		SCCB_SCL(1);
		temp=temp<<1;
		if(SCCB_READ_SDA)temp++;   
		HAL_Delay(1);
		SCCB_SCL(0);
	}	
	SCCB_SDA_OUT();		//设置SDA为输出    
	return temp;
} 							    
//写寄存器
//返回值:0,成功;1,失败.
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data)
{
	uint8_t res=0;
	SCCB_Start(); 					//启动SCCB传输
	if(SCCB_WR_Byte(SCCB_ID))res=1;	//写器件ID	  
	HAL_Delay(1);
  	if(SCCB_WR_Byte(reg))res=1;		//写寄存器地址	  
	HAL_Delay(1);
  	if(SCCB_WR_Byte(data))res=1; 	//写数据	 
  	SCCB_Stop();	  
  	return	res;
}		  					    
//读寄存器
//返回值:读到的寄存器值
uint8_t SCCB_RD_Reg(uint8_t reg)
{
	uint8_t val=0;
	SCCB_Start(); 				//启动SCCB传输
	SCCB_WR_Byte(SCCB_ID);		//写器件ID	  
	HAL_Delay(1);	 
  	SCCB_WR_Byte(reg);			//写寄存器地址	  
	HAL_Delay(1);	  
	SCCB_Stop();   
	HAL_Delay(1);	   
	//设置寄存器地址后，才是读
	SCCB_Start();
	SCCB_WR_Byte(SCCB_ID|0X01);	//发送读命令	  
	HAL_Delay(1);
  	val=SCCB_RD_Byte();		 	//读取数据
  	SCCB_No_Ack();
  	SCCB_Stop();
  	return val;
}

