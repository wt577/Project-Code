#ifndef _SPI_H
#define _SPI_H
#include "sys.h"
#include "stdio.h"	

#define MS5611_CS_H HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET);
#define MS5611_CS_L HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET);
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI1_Read_Write_Byte(uint8_t TxData);
void MX_SPI1_Init(void);
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi);
void MX_GPIO_Init(void);



#endif


