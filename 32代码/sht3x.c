#include "sht3x.h"

static uint8_t SHT31_Send_Cmd(etCommands cmd)
{
	uint8_t cmd_buffer[2];
	cmd_buffer[0] = cmd>>8;
	cmd_buffer[1] = cmd;
	return HAL_I2C_Master_Transmit((I2C_HandleTypeDef *) &hi2c1, (uint16_t) SHT31_ADDR_WRITE, 
		                             (uint8_t *)cmd_buffer, (uint16_t) 2, 
	                               (uint32_t) 999);
}

void SHT31_Reset(void)
{
	SHT31_Send_Cmd(CMD_SOFT_RESET);
	HAL_Delay(20);
}

uint8_t SHT31_Init(void)
{
	return SHT31_Send_Cmd(CMD_MEAS_PERI_2_M);
}

uint8_t SHT31_Read_Dat(uint8_t* dat)
{
	SHT31_Send_Cmd(CMD_FETCH_DATA); 
	return HAL_I2C_Master_Receive((I2C_HandleTypeDef *) &hi2c1, (uint16_t) SHT31_ADDR_READ, 
		                            (uint8_t *) dat, (uint16_t) 6, 
	                              (uint32_t) 999);
}

uint8_t CheckCrc8(uint8_t* const message,uint8_t initial_value)
{
	uint8_t remainder;    //余数
	uint8_t i = 0,j = 0;  //循环变量
	
	/*---初始化----*/
	remainder = initial_value;
	
	for(j = 0;j < 2; j ++)
	{
		remainder ^= message[j];
		
		//从最高位开始依次计算
		for(i = 0;i < 8;i ++)
		{
			if(remainder & 0x80)
			{
				remainder = (remainder << 1)^CRC8_POLYNOMIAL;
			}
			else
			{
				remainder = (remainder << 1);
			}
		}
	}
	//返回计算的CRC码
	return remainder;
}

uint8_t SHT31_Dat_To_Float(uint8_t* const dat,float* temperature,float* humidity)
{
	uint16_t recv_temperature = 0;
	uint16_t recv_humidity = 0;

	/* 校验温度数据和湿度数据是否接收正确 */
	if(CheckCrc8(dat, 0xFF) != dat[2] || CheckCrc8(&dat[3], 0xFF) != dat[5])
			return 1;

	/* 转换温度数据 */
	recv_temperature = ((uint16_t)dat[0]<<8)|dat[1];
	*temperature = -45 + 175*((float)recv_temperature/65535);

	/* 转换湿度数据 */
	recv_humidity = ((uint16_t)dat[3]<<8)|dat[4];
	*humidity = 100 * ((float)recv_humidity / 65535);

	return 0;
}
