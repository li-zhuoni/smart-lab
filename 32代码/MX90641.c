#include "main.h"
#include "i2c.h"


void start_bit(void)
{
	SDA = 1;
	HAL_delay(5);
	SCL = 1;
	HAL_delay(5);
	SDA = 0;
	HAL_delay(5);
	SCL = 0;
	HAL_delay(5);
}

void stop_bit(void)
{
	SCL = 0;
	HAL_delay(5);
	SDA = 0;
	HAL_delay(5);
	SCL = 1;
	HAL_delay(5);
	SDA = 1;
}

void send_bit(void)
{
	if(bit_out==0)
		SDA = 0;
	else 
		SDA = 1;
	HAL_delay(1);
	SCL = 1;
	HAL_delay(8);
	SCL = 0;
	HAL_delay(8);
}

void receive_bit(void)
{
	SDA = 1;
	bit_in = 1;
	SCL = 1;
	HAL_delay(8);
	bit_in = SDA;
	HAL_delay(1);
	SCL = 0;
	HAL_delay(8);
}

void tx_byte(uint8_t dat_byte)
{
	uint8_t i,n,dat;
	n = Nack_counter;
	Tx_again:
	dat = dat_byte;
	for(i=0;i<8;i++)
	{
		if(dat&0x80)
			bit_out = 1;
		else 
			bit_out = 0;
		send_bit();
		dat = dat<<1;
	}
	receive_bit();
	if(bit_in == 1)
	{
		stop_bit();
		if(n!=0)
		{
			n--;
			goto Repeat;
		}else 
		{
			goto exit;
		}
	}else 
	{
		goto exit;
	}
	
	Repeat:
	start_bit();
	goto Tx_again;
	
	exit:;
}

void rx_byte(void)
{
	uint8_t i,dat;
	dat = 0;
	for(i=0;i<8;i++)
	{
		dat = dat << 1;
		receive_bit();
		if(bit_in = 1)  
			dat = dat+1;
	}
	send_bit();
	return dat;
}

uint16_t menmread(void)
{
	/*
	start_bit();
	tx_byte(0x00); //send slave address
	tx_byte(0x07);  //send master address
	
	start_bit();
	tx_byte(0x01);
	*/
	HAL_I2C_Master_Transmit((I2C_HandleTypeDef *) &hi2c2, (uint16_t) 0x00, 
		                             (uint8_t *)0x01, (uint16_t) 2, 
	                               (uint32_t) 999);
	
	bit_out = 0;
	DataL = rx_byte();
	bit_out = 0;
	DataH = rx_byte();
	bit_out = 1;
	Pecreg = rx_byte();
	stop_bit();
	return(DataH*256+DataL);
}

