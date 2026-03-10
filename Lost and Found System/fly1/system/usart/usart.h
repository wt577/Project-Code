#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"
#include "stdbool.h"
#define RX_BUFFER_SIZE 1024
#define USART1_REC_LEN  			1024  	//定义最大接收字节数 200
#define USART2_REC_LEN  			1024  	//定义最大接收字节数 200
#define USART3_REC_LEN  			1024  	//定义最大接收字节数 200
#define USARTg_REC_LEN  			1024  	//定义最大接收字节数 200
#define USARTy_REC_LEN  			1024  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

extern char  USART_RX_BUF[USART1_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern u16 USART_RX_STA;         		//接收状态标记

extern u8  USART3_RX_BUF[USART3_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART3_RX_STA;         		//接收状态标记	

extern u8  USARTg_RX_BUF[USARTg_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USARTg_RX_STA; 

extern u8  USARTy_RX_BUF[USARTy_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USARTy_RX_STA; 

extern uint8_t  flag;

#define false 0
#define true 1

//定义数组长度
#define GPS_Buffer_Length 80
#define UTCTime_Length 11
#define latitude_Length 11
#define N_S_Length 2
#define longitude_Length 12
#define E_W_Length 2 

typedef struct SaveData 
{
	char GPS_Buffer[GPS_Buffer_Length];
	char isGetData;		//是否获取到GPS数据
	char isParseData;	//是否解析完成
	char UTCTime[UTCTime_Length];		//UTC时间
	char latitude[latitude_Length];		//纬度
	char N_S[N_S_Length];		//N/S
	char longitude[longitude_Length];		//经度
	char E_W[E_W_Length];		//E/W
	char isUsefull;		//定位信息是否有效
	char temp[10];
	char data[5];
	char isGetAT;		//是否获取到GPS数据
} _SaveData;


void uart_init(u32 bound);
extern char rxdatabufer;
extern u16 point1;
extern u16 point2;
extern u16 point3;
extern u16 point4;
extern _SaveData Save_Data;

extern char old_data1[20];
extern char old_data2[20];

extern __attribute__((section(".dma_buffer"))) uint8_t dma_rx_buffer[USART3_REC_LEN]; // DMA专用缓冲区

extern IWDG_HandleTypeDef hiwdg1;
extern DMA_HandleTypeDef DMA1_TX_Handler;

extern UART_HandleTypeDef UART1_Handler; //UART句柄
extern UART_HandleTypeDef UART2_Handler; //UART句柄
extern UART_HandleTypeDef UART3_Handler; //UART句柄
extern char Rx_Counter;
extern char Rx_Buffer[200];     //接收缓冲,最大100个字节.
extern u8 flag_status,flag_battery;

#define RXBUFFERSIZE   1 //缓存大小
extern u8 aRxBuffer[RXBUFFERSIZE];//HAL库USART接收Buffer

//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);
bool Uart2_SendNonBlocking(const uint8_t *data, uint16_t len);
void CLR_Buf(void);
u8 Hand(char *a);
void clrStruct(void);
#endif


