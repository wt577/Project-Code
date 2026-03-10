#ifndef __UART_H
#define __UART_H
#include "stm32L4xx.h" 
#include "stm32L4xx_hal_uart.h" 
#include "sys.h"
#include "stdbool.h"
#define USART_REC_LEN  			1024  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	 
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
extern u8  USART3_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART3_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);
bool Uart3_SendNonBlocking(const uint8_t *data, uint16_t len);

#endif
