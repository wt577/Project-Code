#include "exti.h"
#include "delay.h"
#include "led.h"
//#include "key.h"
#include "buzzer.h"
#include "i2c.h"
/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	本程序只供学习使用，未经作者许可，不得用于其它任何用途
 *	ALIENTEK Pandora STM32L475 IOT开发板
 *	外部中断驱动代码
 *	正点原子@ALIENTEK
 *	技术论坛:www.openedv.com
 *	创建日期:2018/10/27
 *	版本：V1.0
 *	版权所有，盗版必究。
 *	Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 *	All rights reserved
 *	******************************************************************************
 *	初始版本
 *	******************************************************************************/

/**
 * @brief	外部中断初始化函数
 *
 * @param   void
 *
 * @return  void
 */
void EXTI_Init(void)
{
	/*
		KEY0 	- PD10
		KEY1 	- PD9
		KEY2 	- PD8
		WK_UP 	- PC13
	*/
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();               //开启GPIOC时钟
    
    GPIO_Initure.Pin=GPIO_PIN_5;	//PB5
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发GPIO_MODE_IT_FALLING
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
    
    //中断线5
    HAL_NVIC_SetPriority(EXTI9_5_IRQn,2,0);       //抢占优先级为2，子优先级为0
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);             //使能中断线0

}

/**
 * @brief	EXTI9_5中断服务函数
 *
 * @param   void
 *
 * @return  void
 */
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);//调用中断处理公用函数
	
}

/**
 * @brief	中断服务程序中需要做的事情,在HAL库中所有的外部中断服务函数都会调用此函数
 *
 * @param   GPIO_Pin	中断引脚号
 *
 * @return  void
 */
u8 delay_180ms=0;
u8 FLAG=0;
uint8_t temp[1]={0x11};//连续高分辨率模式2
float lux=0;
uint8_t ReadData[2]={0};
char str[100];
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
		
    if(GPIO_Pin==GPIO_PIN_5)
		{
			if(FLAG)
			{
				  if(delay_180ms==0)HAL_I2C_Master_Transmit(&hi2c3, 0x46, temp, 1, 0xff);

					if(delay_180ms==35)
					{		
						delay_180ms=0;
						HAL_I2C_Master_Receive(&hi2c3, 0x47,ReadData, 2, 0xff);
						lux=(float)((ReadData[0]<<8)|ReadData[1]);
						lux=(double)lux/1.2;
						FLAG=0;
//						sprintf(str, "lux=%f\r\n", lux);
//						Uart3_SendStr(str);
					}
					delay_180ms++;
			}

			  
			
		}
	
}

/**
 将浮点型数据装换成字符串数据
 */

void Float2String(double lat,double lon)
{
	char  str2[70];
	int lat_Head = (int)lat;
	int lat_Point = (int)((lat - lat_Head)*100000000.0);
  int lon_Head = (int)lon;
	int lon_Point = (int)((lon - lon_Head)*100000000.0);
    sprintf(str2,"AT+MIPLNOTIFY=0,0,3336,0,5513,1,24,\"%d.%d,%d.%d\",0,0\r\n",lat_Head,lat_Point,lon_Head,lon_Point);
//    Uart3_SendStr(str2);
}


