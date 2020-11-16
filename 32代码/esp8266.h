#ifndef __ESP8266_H
#define __ESP8266_H
#include "sys.h"
#include "stdio.h"	
#include "main.h"




//º¯Êı

uint8_t esp8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime);
uint8_t* esp8266_send_data(uint8_t *cmd,uint16_t waittime);
uint8_t* esp8266_check_cmd(uint8_t *str);
void esp8266_start_trans(void);
uint8_t esp8266_quit_trans(void);	
#endif
