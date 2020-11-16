#include "sys.h"
#include "sccb.h"
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

//��ʼ��SCCB�ӿ� 
void SCCB_Init(void)
{				
  /*
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIODʱ��
	
  //GPIOF9,F10��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;//PD6,7 �������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //PD6,7 �������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��
 
	GPIO_SetBits(GPIOD,GPIO_Pin_6|GPIO_Pin_7);
	SCCB_SDA_OUT();	   
	*/
	
	GPIO_InitTypeDef GPIO_InitStructure;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStructure.Pin = GPIO_PIN_10|GPIO_PIN_11;//PDB10,11 �������
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;  //PB10,11 �������
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;//100MHz
  GPIO_InitStructure.Pull = GPIO_PULLUP;//����
  HAL_GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10|GPIO_PIN_1,1);
	SCCB_SDA_OUT();	
}			 

//SCCB��ʼ�ź�
//��ʱ��Ϊ�ߵ�ʱ��,�����ߵĸߵ���,ΪSCCB��ʼ�ź�
//�ڼ���״̬��,SDA��SCL��Ϊ�͵�ƽ
void SCCB_Start(void)
{
    SCCB_SDA(1);     //�����߸ߵ�ƽ	   
    SCCB_SCL(1);	    //��ʱ���߸ߵ�ʱ���������ɸ�����
    HAL_Delay(1);  
    SCCB_SDA(0);
    HAL_Delay(1);	 
    SCCB_SCL(0);	    //�����߻ָ��͵�ƽ��������������Ҫ	  
}

//SCCBֹͣ�ź�
//��ʱ��Ϊ�ߵ�ʱ��,�����ߵĵ͵���,ΪSCCBֹͣ�ź�
//����״����,SDA,SCL��Ϊ�ߵ�ƽ
void SCCB_Stop(void)
{
    SCCB_SDA(0);
    HAL_Delay(1);	 
    SCCB_SCL(1);	
    HAL_Delay(1); 
    SCCB_SDA(1);	
    HAL_Delay(1);
}  
//����NA�ź�
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
//SCCB,д��һ���ֽ�
//����ֵ:0,�ɹ�;1,ʧ��. 
uint8_t SCCB_WR_Byte(uint8_t dat)
{
	uint8_t j,res;	 
	for(j=0;j<8;j++) //ѭ��8�η�������
	{
		if(dat&0x80)SCCB_SDA(1);	
		else SCCB_SDA(0);
		dat<<=1;
		HAL_Delay(1);
		SCCB_SCL(1);	
		HAL_Delay(1);
		SCCB_SCL(0);		   
	}			 
	SCCB_SDA_IN();		//����SDAΪ���� 
	HAL_Delay(1);
	SCCB_SCL(1);			//���յھ�λ,���ж��Ƿ��ͳɹ�
	HAL_Delay(1);
	if(SCCB_READ_SDA)res=1;  //SDA=1����ʧ�ܣ�����1
	else res=0;         //SDA=0���ͳɹ�������0
	SCCB_SCL(0);		 
	SCCB_SDA_OUT();		//����SDAΪ���    
	return res;  
}	 
//SCCB ��ȡһ���ֽ�
//��SCL��������,��������
//����ֵ:����������
uint8_t SCCB_RD_Byte(void)
{
	uint8_t temp=0,j;    
	SCCB_SDA_IN();		//����SDAΪ����  
	for(j=8;j>0;j--) 	//ѭ��8�ν�������
	{		     	  
		HAL_Delay(1);
		SCCB_SCL(1);
		temp=temp<<1;
		if(SCCB_READ_SDA)temp++;   
		HAL_Delay(1);
		SCCB_SCL(0);
	}	
	SCCB_SDA_OUT();		//����SDAΪ���    
	return temp;
} 							    
//д�Ĵ���
//����ֵ:0,�ɹ�;1,ʧ��.
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data)
{
	uint8_t res=0;
	SCCB_Start(); 					//����SCCB����
	if(SCCB_WR_Byte(SCCB_ID))res=1;	//д����ID	  
	HAL_Delay(1);
  	if(SCCB_WR_Byte(reg))res=1;		//д�Ĵ�����ַ	  
	HAL_Delay(1);
  	if(SCCB_WR_Byte(data))res=1; 	//д����	 
  	SCCB_Stop();	  
  	return	res;
}		  					    
//���Ĵ���
//����ֵ:�����ļĴ���ֵ
uint8_t SCCB_RD_Reg(uint8_t reg)
{
	uint8_t val=0;
	SCCB_Start(); 				//����SCCB����
	SCCB_WR_Byte(SCCB_ID);		//д����ID	  
	HAL_Delay(1);	 
  	SCCB_WR_Byte(reg);			//д�Ĵ�����ַ	  
	HAL_Delay(1);	  
	SCCB_Stop();   
	HAL_Delay(1);	   
	//���üĴ�����ַ�󣬲��Ƕ�
	SCCB_Start();
	SCCB_WR_Byte(SCCB_ID|0X01);	//���Ͷ�����	  
	HAL_Delay(1);
  	val=SCCB_RD_Byte();		 	//��ȡ����
  	SCCB_No_Ack();
  	SCCB_Stop();
  	return val;
}

