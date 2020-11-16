// Define to prevent recursive inclusion  --------------------------------------
#define _IOT_C_

// Includes  -------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "stm32f40x.h"

#include "drv.h"
#include "iot.h"
#include "hts.h"
#include "lps.h"
#include "lsm.h"
#include "resource.h"
#include "hal_gpio.h"
#include "hal_uart.h"
#include "common.h"
#include "drv_uart.h"
#include "sensor_common.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_uart.h"


#define ElemType u8
#define MAXSIZE 100
#define BOOL int
#define FALSE 0
#define TRUE  1

#define MY_ID  0x44

typedef struct{
    ElemType data[MAXSIZE];
    int front, rear;
}Queue;

void InitQueue(Queue *queue);   //初始化
int IsEmpty(Queue *queue); //判空
void EnQueue(Queue *queue, ElemType x); //入队
void DeQueue(Queue *queue, ElemType *x);    //出队
void DeTail(Queue *queue, ElemType *e); //队尾弹出


void InitQueue(Queue *queue){
    queue->front = 0;
    queue->rear = 0;
}

void EnQueue(Queue *queue, ElemType e){
    queue->data[queue->rear] = e;
    queue->rear = (queue->rear + 1) % MAXSIZE;
}

void DeQueue(Queue *queue, ElemType *e){
    *e = queue->data[queue->front];
    queue->front = (queue->front + 1) % MAXSIZE;
}

int IsEmpty(Queue *queue){
    if (queue->front == queue->rear) {
        return 1;
    } else {
        return 0;
    }
}
void DeTail(Queue *queue, ElemType *e){
    *e = queue->data[queue->rear -1];
    queue->rear = (queue->rear - 1) % MAXSIZE;
}

u8 Make_report(u8* tempStr,u8* humStr,u8* pressStr,u8* SenSor,Queue *in,Queue *out)
{
	
	/* Protocol  table */
	
	EnQueue(out,0x66); //Request header
	EnQueue(out,0x01); //Request Type
	EnQueue(out,MY_ID); //Area ID
	EnQueue(out,tempStr[0]); //Sensor 	Tem 
	EnQueue(out,tempStr[1]); //Sensor 	Tem
	EnQueue(out,humStr[0]); //Sensor 		Hum
	EnQueue(out,humStr[1]); //Sensor 		Hum
	EnQueue(out,pressStr[0]); //Sensor 	Press
	EnQueue(out,pressStr[1]); //Sensor 	Press
	EnQueue(out,pressStr[2]); //Sensor 	Press
	EnQueue(out,pressStr[3]); //Sensor 	Press
	EnQueue(out,pressStr[4]); //Sensor 	Press
	EnQueue(out,pressStr[5]); //Sensor 	Press
	EnQueue(out,SenSor[0]); //Sensor k1
	EnQueue(out,SenSor[1]); //Sensor k2
	EnQueue(out,SenSor[2]); //Sensor k3
	EnQueue(out,SenSor[3]); //Sensor k4
	EnQueue(out,0xFF); //Sensor X
	EnQueue(out,0xFF); //Sensor X
	EnQueue(out,0xFF); //Sensor X

	u8 flag = 0x00;
	while(!IsEmpty(in)){
		ElemType data;
		DeQueue(in, &data);
			if(data == 0xBB && flag == 0x00){
				flag = 0x01;
				EnQueue(out, data);
				ElemType e;
				DeQueue(in, &e);
				if(e == 0x01){
						EnQueue(out,0x00);
						EnQueue(out,0x7E);
						flag = 0x00;
				}
				else
				for(u8 i=0;i<6;i++) {
					DeQueue(in, &e);
				}
			}
			else if(data == 0x7E && flag == 0x01){
				flag = 0x00;
				for(u8 i=0;i<3;i++){
					ElemType e;
					DeTail(out,&e);
				}
				EnQueue(out, data);
				EnQueue(out, MY_ID); // Area_2_id
			}
			else if(flag == 0x01){
				EnQueue(out, data);
			}
	}
	EnQueue(out,0x88); //Request end
	
	/* Protocol  table  END*/
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AppTaskTick()
{
    if (tickCnt++ >= 3600000) {
        tickCnt  = 0;
        tickFlag = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  set delay time
/// @param  ms: delay time
/// @retval bool: true: delay ms time
////////////////////////////////////////////////////////////////////////////////
bool delay(u16 ms)
{
    static u16 cnt = 0;
    if (cnt == 0) {
        cnt = ms;
    }
    else if (sysTickFlag1mS) {
        sysTickFlag1mS = false;
        if (cnt > 0)
            cnt--;
        if (cnt == 0)
            return true;
    }
    return false;
}
void Delay(unsigned int xms) // xms??????????

{

unsigned int x,y;

for(x=xms;x>0;x--)

for(y=110;y>0;y--);

}
	



#if 1
void UART_Configure()
{
    UART_InitTypeDef InitStructure;

    InitStructure.BaudRate      = 115200;
    InitStructure.WordLength    = UART_WordLength_8b;
    InitStructure.StopBits      = UART_StopBits_1;
    InitStructure.Parity        = UART_Parity_No;
    InitStructure.HWFlowControl = UART_HWFlowControl_None;
    InitStructure.Mode          = (EM_UARTMODE)(UART_Mode_Rx | UART_Mode_Tx);
    InitStructure.Mode         &= ~UART_GCR_DMA;

    COMMON_EnableIpClock(emCLOCK_UART1);
    COMMON_EnableIpClock(emCLOCK_UART2);

    UART_Init(UART1, &InitStructure);
    UART_Init(UART2, &InitStructure);
    UART_Cmd(UART1, ENABLE);
    UART_Cmd(UART2, ENABLE);

    BSP_UART_GPIO_Configure(UART1, 0, 0);
    BSP_UART_GPIO_Configure(UART2, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  numbers transform to string
////////////////////////////////////////////////////////////////////////////////
void NumtoStr(u32 value, u8* str, u8 len)
{
    for (u8 i = 0;i < len;i++) {
        str[len - i - 1] = ((value % 16) > 9) ? (value % 16) + 55 : (value % 16) + 48;
        value >>= 4;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Combine two string together
////////////////////////////////////////////////////////////////////////////////
void stract(u8 *dest, u8 *src,u8 len)
{
    while (*dest)
        dest++;
    for(u8 i = 0;i < len;i++) {
        *dest = *src;
        dest++;
        src++;
    }
}
////////////////////////////////////////////////////////////////////////////////
/// @brief  Combine two string together
////////////////////////////////////////////////////////////////////////////////
void stract2(u8 *dest, u8 date)
{
    while (*dest)
        dest++;
				*dest = date;   
}

void hardwareCfg()
{
    BSP_LED_Configure();
    BSP_KEY_Configure();
    memset(&KEYB, 0x00, sizeof(KEYB));
    GPIO_SetBits(GPIOB, GPIO_Pin_0);

    LD3_on();
    LD4_on();
    LD5_on();
    LD6_on();
    CloseLED();

    if (HTS221_Init())  LD4_on();
    if (LPS22HB_Init()) LD5_on();
    if (LSM6DSL_Init()) LD6_on();

    HTS221_Get_Equation_Temp();
    HTS221_Get_Equation_Hum();
    LPS22HB_Config();
}

//bool checkCommond()
//{
//    u8 result[32];
//    u8 i = 0;
//    while(1) {
//        if(UART_GetITStatus(UART2, UART_IT_RXIEN)) {
//            result[i] = UART_ReceiveData(UART2);
//            UART_ClearITPendingBit(UART2, UART_IT_RXIEN);
//            if ((result[i] == 'K') && (result[i-1] == 'O'))
//                return true;
//            else if ((result[i] == 'R') && (result[i-1] == 'O') && (result[i-2] == 'R') && (result[i-3] == 'R') && (result[i-4] == 'E'))
//                return false;
//            i++;
//            if (i == 255) return false;
//        }
//    }
//}

//const AT_Commond ATCommond[] = {
//    ATCOMMOND("AT+CMEE=1\r\n", 11),
//    ATCOMMOND("AT+CFUN=0\r\n", 11),
//    ATCOMMOND("AT+CGSN=1\r\n", 11),
//    ATCOMMOND("AT+NCDP=180.101.147.115,5683\r\n", 30),
//    ATCOMMOND("AT+CFUN=1\r\n", 11),
//    ATCOMMOND("AT+CGATT=1\r\n", 12),
//    ATCOMMOND("AT+CGPADDR\r\n", 12)
//};
//
//bool connectResult[7];
//
//bool connectIoT()
//{
//    u8* str;
//    for (u8 i = 0;i < 7;i++) {
//        str = ATCommond[i].str;
//        for (u8 j = 0;j < ATCommond[i].len;j++) {
//            UART_SendData(UART2, *str++);
//            while (!UART_GetITStatus(UART2, UART_IT_TXIEN));
//        }
//        if(checkCommond())
//            connectResult[i] = true;
//        else return false;
//    }
//    return true;
//}

float avTemp;
float avHum;
double press;

u8 tempStr[2];
u8 humStr[2];
u8 pressStr[6];
u8 SenSor[4];

u8 txCommond[12] = "AT+NMGS=6,00";
u8 rxCommond[22] = "AT+NMGS=5,";
u8 txBuffer[24];
u8 rxBuffer[22];


void getSensorInfo()
{
    avTemp = HTS221_Get_AVTemp();
    avHum = HTS221_Get_AVHum();
    press = LPS22HB_Get_Press() * 100;

    NumtoStr((u8)avTemp, tempStr, 2);
    NumtoStr((u8)avHum, humStr, 2);
    NumtoStr((u32)press, pressStr, 6);

}

/*发送一个字节*/
void Uart_SendByte(UART_TypeDef* pUARTx,uint8_t data)
{
	UART_SendData(pUARTx, data);
	while (UART_GetFlagStatus(pUARTx, UART_FLAG_TXEMPTY) == RESET);
}
/*发送字符串str*/
void Uart_SendStr(UART_TypeDef* pUARTx,Queue *str)
{
	while(!IsEmpty(str)){
		ElemType e;
		DeQueue(str, &e);
		Uart_SendByte(pUARTx,e);
		while(UART_GetFlagStatus(pUARTx, UART_FLAG_TXEMPTY) == RESET);
	}
}
void Uart_SendArray(UART_TypeDef* pUARTx,uint8_t *array,uint8_t num)
{
	uint8_t i;
	for(i = 0;i<num;i++)
	{
		Uart_SendByte(pUARTx,array[i]);
	}
	while(UART_GetFlagStatus(pUARTx, UART_FLAG_TXEMPTY) == RESET);
}


/*接收一个字节并回发*/
void Uart_ReceiveByte(UART_TypeDef* pUARTx)
{
		uint8_t data;
	/* 等待串口输入数据 */
		while (UART_GetFlagStatus(pUARTx, UART_FLAG_RXAVL) == RESET);
		data = UART_ReceiveData(pUARTx);
}

///*接收一个字节并追加到字符串*/
//u8 Uart_ReceiveByte_Add_T0_Str(UART_TypeDef* pUARTx,Queue* str)
//{
//	u8 flag1 = 1;
//	while(UART_GetFlagStatus(pUARTx, UART_FLAG_RXAVL) == SET)
//	{
//		uint8_t data;
//		data = UART_ReceiveData(pUARTx);
//		EnQueue(str, data);
//	}	
//	return 1;
//}

/*接收一个字节并追加到字符串*/
u8 Uart_ReceiveStr_1(UART_TypeDef* pUARTx,Queue* str)
{
	u8 flag = 0x01;
	u8 button = 0x01;
	while(flag){
		if(button){
			flag = 0x00;
			button = 0x00;
		}
		while(UART_GetFlagStatus(pUARTx, UART_FLAG_RXAVL) == RESET);
		while(UART_GetFlagStatus(pUARTx, UART_FLAG_RXAVL) != RESET){
			u8 data = UART_ReceiveData(pUARTx);
			if(data == 0x66 && flag == 0x00){
				flag = 0x01;
				EnQueue(str, data);
			}
			else if(data == 0x88 && flag == 0x01){
				flag = 0x00;
				EnQueue(str, data);
			}
			else if(flag == 0x01){
				EnQueue(str, data);		
			}
			else if(flag == 0x00){
				flag = 0x01;
				button = 0x01;
			}
		}
	}
	return 0;
}
u8 Uart_ReceiveStr_2(UART_TypeDef* pUARTx,Queue* str)
{
	u8 flag = 0x01;
	u8 button = 0x01;
	int i =0;
	while(1){
		if(button){
			flag = 0x00;
			button = 0x00;
		}
		while(UART_GetFlagStatus(pUARTx, UART_FLAG_RXAVL) == RESET){
			if(i>150000) return 0;
			i++;
		}
		i=0;
		while(UART_GetFlagStatus(pUARTx, UART_FLAG_RXAVL) != RESET){
			u8 data = UART_ReceiveData(pUARTx);
			if(data == 0xBB && flag == 0x00){
				flag = 0x01;
				EnQueue(str, data);
			}
			else if(data == 0x7E && flag == 0x01){
				flag = 0x00;
				EnQueue(str, data);
			}
			else if(flag == 0x01){
				EnQueue(str, data);
			}
			else if(flag == 0x00){
				flag = 0x01;
				button = 0x01;
			}

		}
	}
}


void Send_instructions(){
	u8 ins[] = {0xBB,0x00,0x22,0x00,0x00,0x22,0x7E};
	Uart_SendArray(UART1,ins,0x07);
}

u8 Is_me(Queue *str)
{
	int i=0;
	while(!IsEmpty(str)){
		ElemType e;
		DeQueue(str, &e);
		if(i == 2 && e == MY_ID) return 1;
		i++;
	}
	return 0;
}


//!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!
int main(void)
{
    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);

    UART_Configure();
    hardwareCfg();
		

		Delay(10000);
	while(1)
	{
		//Receive commands (PI)
		Queue a1;
		Queue* q1 = &a1;
		InitQueue(q1);
		Uart_ReceiveStr_1(UART2,q1);
		if(!Is_me(q1)) continue;
		//Send the command and return (RFID)
		Queue a2;
		Queue* q2 = &a2;
		InitQueue(q2);
		Send_instructions();// to U1
		Uart_ReceiveStr_2(UART1,q2);
		//To make the report
		Queue a3;
		Queue* q3 = &a3;
		InitQueue(q3);
		//get Tem,Hum,Press
		memset(tempStr, 0, 2);
		memset(humStr, 0, 2);
		memset(pressStr, 0, 6);
		getSensorInfo();
		//get k4 k3 k2 k1
		//gpio引脚接地时灯灭，相应数据为0
		memset(SenSor, 0, 4);
		//k4
		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15)){//
			LD6_on();
			SenSor[3] = 1;
		}
		else {
			LD6_off();
			SenSor[3] = 0;
		}
		//
		//k3
		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14)){//
			LD5_on();
			SenSor[2] = 1;
		}
		else {
			LD5_off();
			SenSor[2] = 0;
		}
		//
		//k2
		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)){//
			LD4_on();
			SenSor[1] = 1;
		}
		else {
			LD4_off();
			SenSor[1] = 0;
		}
		//
		//Put to sever
		Make_report(tempStr,humStr,pressStr,SenSor,q2,q3);
		Uart_SendStr(UART2,q3);
		}
	}

#endif



