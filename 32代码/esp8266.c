#include "esp8266.h"
#include "string.h"
#include "usart.h"
#include "sys.h" 
#include "delay.h"

uint16_t ack_count = 0;

extern __IO uint16_t USART2_RX_STA;
extern uint8_t USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
extern uint8_t  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 			//发送缓冲,最大USART3_MAX_SEND_LEN字节

void esp8266_start_trans(void)
{
	//还原模块为出厂设置
	esp8266_send_cmd("AT+RESTORE\r\n","OK",500);
	delay_ms(700);         //延时3S等待重置成功
	//设置为station模式
	esp8266_send_cmd("AT+CWMODE=1\r\n","OK",300);
  delay_ms(100); 
	//设置工作模式 1：station模式   2：AP模式  3：兼容 AP+station模式
	esp8266_send_cmd("AT+CWJAP_DEF=\"mix3\",\"zhuzhengyu1123\"\r\n","GOT",3000); 
    //AP模式
	delay_ms(5500);
	esp8266_send_cmd("AT+CIFSR\r\n","OK",300);
	
	esp8266_send_cmd("AT+CIPMUX=1\r\n","OK",300);
	esp8266_send_cmd("AT+RST\r\n","OK",3000);
	esp8266_send_cmd("AT+CIPSTART=\"TCP\",\"192.168.43.1\",8001\r\n","OK",300);
}
//向ESP8266发送命令
//cmd:发送的命令字符串;ack:期待的应答结果,如果为空,则表示不需要等待应答;waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果);1,发送失败
uint8_t esp8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
	uint8_t res=0; 
	USART2_RX_STA=0;
	printf("发送指令之前\r\n");
	//u2_printf("%s",cmd);	//发送命令
	//while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
	HAL_UART_Transmit(&huart2,cmd,sizeof(cmd)-1,1000);
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	   //等待倒计时
		{
			printf("%d",ack_count++);
			if(USART2_RX_STA&0X8000)//接收到期待的应答结果
			{
				printf("开始检查应答");
				if(esp8266_check_cmd(ack))
				{
					printf("ack:%s\r\n",(uint8_t*)ack);
					break;//得到有效数据 
				}
					USART2_RX_STA=0;
			} 
		}
		if(waittime==0){res=1; printf("未接受到应答\r\n");}		
	}
	ack_count = 0;  //

	printf("\r\n");
	return res;
} 
//ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果;其他,期待应答结果的位置(str的位置)
uint8_t* esp8266_check_cmd(uint8_t *str)
{
	uint8_t *strx=0;
	if(USART2_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
	} 
	printf("%s",strx);
	return (uint8_t*)strx;
}



void ESP01_Getweb(void)
{
    delay_ms(50);
    
    printf("AT+CWMODE=1");
    delay_ms(50);
    RST_ON;
    delay_ms(100);
    RST_OFF;
    delay_ms(50);
    
    USART_RX_STA2=0x0000;
    printf("AT+CWJAP=\"WIFI name\",\"password\"");
    delay_ms(1000);  //注意delay时间限制
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
}

void ESP01_Gettime(void)
{
    printf("AT+CIPMUX=0\r\n");
    delay_ms(500);
    
    printf("AT+CIPSTART=\"TCP\",\"api.k780.com\",80\r\n");
    delay_ms(1600);
    
    printf("AT+CIPMODE=1\r\n");
    delay_ms(500);
    
    printf("AT+CIPSEND\r\n");
    delay_ms(500);
    USART_RX_STA2=0x0000;  //使数据从数组头开始记录
    printf("GET http://api.k780.com:88/?app=life.time&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json&HTTP/1.1\r\n");
    delay_ms(1600);
}


