#include "ms5611.h"
#include "sys.h"
#include "delay.h"
#include "math.h"
#include "FreeRTOS.h"
#include "task.h"
uint16_t data[8];
float temperature,TEMP2,Aux,OFF2,SENS2;
float  tmp_float,Altitude;
	int32_t pressure_temp;
void ms5611_ba_reset(void)
{
	MS5611_CS_L;
	SPI1_Read_Write_Byte(0X1E);
	MS5611_CS_H;
	HAL_Delay(20);
}

uint16_t ms5611_read(u8 cmd)
{
	u8 temp1,temp2;
	MS5611_CS_L;
	SPI1_Read_Write_Byte(cmd);
	temp1 = SPI1_Read_Write_Byte(0xff);
	temp2 = SPI1_Read_Write_Byte(0xff);
	MS5611_CS_H;
	
	return (temp1<<8)|temp2;
}

uint32_t ms5611_read_adc(u8 cmd)
{
	u8 temp1,temp2,temp3;
	MS5611_CS_L;
	SPI1_Read_Write_Byte(cmd);
	temp1 = SPI1_Read_Write_Byte(0xff);
	temp2 = SPI1_Read_Write_Byte(0xff);
	temp3 = SPI1_Read_Write_Byte(0xff);
	MS5611_CS_H;
	
	return (temp1<<16)|(temp2<<8)|temp3;
}

void read_prom_data(void)
{
	u8 i;
	for(i=0;i<8;i++)
		data[i] = ms5611_read(0xa0+2*i);
	for(i=0;i<8;i++);
}


void ms5611_init(void)
{
	ms5611_ba_reset();
	read_prom_data();
}

float get_Altitude(void )
{
	pressure_temp = get_pressure();//气压计
	tmp_float = (pressure_temp / 101500.0);
	tmp_float = pow(tmp_float, 0.190295);               
	Altitude = 44330 * (1.0 - tmp_float);
	return Altitude;
}
float get_temp(void)
{
	int32_t temp;
	MS5611_CS_L;
	SPI1_Read_Write_Byte(0X58);
	MS5611_CS_H;
	HAL_Delay(10);
	temp = ms5611_read_adc(0x00);
	temp -= (data[5]<<8);
	temperature = 2000 + temp*data[6]/8388608;
	return temperature;
}

int32_t get_pressure(void)
{
	int64_t D1,/*dT,*/OFF,SENS;
	int64_t pressure;
	
	MS5611_CS_L;
	SPI1_Read_Write_Byte(0X58);
	MS5611_CS_H;
	HAL_Delay(9);
	
//	dT = ms5611_read_adc(0x00);
//	dT -= (data[5]<<8);
	
	int32_t dT = ms5611_read_adc(0x00); // 读取温度ADC原始值
    dT -= ((int32_t)data[5] << 8); // 修正符号处理
	
	OFF = ((int64_t)data[2]<<16) + (((int64_t)data[4]*dT)>>7);
	SENS = ((int64_t)data[1]<<15) + (((int64_t)data[3]*dT)>>8);

	MS5611_CS_L;
	SPI1_Read_Write_Byte(0x48);
	MS5611_CS_H;
	 HAL_Delay(8);
	D1 = ms5611_read_adc(0x00);
	if(temperature<2000)
  {
    // second order temperature compensation when under 20 degrees C
    TEMP2 = (dT*dT) / 0x80000000;
    Aux = temperature*temperature;
    OFF2 = 2.5f*Aux;
    SENS2 = 1.25f*Aux;
    temperature = temperature - TEMP2;
    OFF = OFF - OFF2;
    SENS = SENS - SENS2; 
  }
	pressure = (((D1*SENS)>>21)-OFF)>>15;
	return pressure;
}



