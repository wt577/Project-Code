#include "usart.h"
#include "delay.h"
#include "stdbool.h"
#include "buzzer.h"

#define RX_BUF_SIZE 1024  // ???????
#define TX_BUF_SIZE 256   // ???????

char rxdatabufer;
u16 point1 = 0;
u16 point2 = 0;
u16 point3 = 0;
u16 point4 = 0;
u8 flag_status=0,flag_battery=0;
_SaveData Save_Data;
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
 *	串口1初始化
 *	正点原子@ALIENTEK
 *	技术论坛:www.openedv.com
 *	修改日期:2015/9/7
 *	版本：V1.5
 *	版权所有，盗版必究。
 *	Copyright(C) 广州市星翼电子科技有限公司 2009-2024
 *	All rights reserved
 *	******************************************************************************
 *	V1.0修改说明
 *	加入以下代码,支持printf函数,而不需要选择use MicroLIB
 *	******************************************************************************/


#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;
};

FILE __stdout;
/**
 * @brief	定义_sys_exit()以避免使用半主机模式
 */
void _sys_exit(int x)
{
    x = x;
}
/**
 * @brief	重定义fputc函数
 */
int fputc(int ch, FILE *f)
{
		while((USART2->ISR & 0X40) == 0); //循环发送,直到发送完毕

		USART2->TDR = (u8) ch;
    return ch;
}
#endif


#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
char USART_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART3_RX_BUF[USART3_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USARTg_RX_BUF[USARTg_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USARTy_RX_BUF[USARTy_REC_LEN];  
u8 num1=0,num2=0,num3=0;

char old_data1[20];
char old_data2[20];
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记
u16 USART2_RX_STA=0;       //接收状态标记
u16 USART3_RX_STA=0; 
u16 USARTg_RX_STA=0; 
u16 USARTy_RX_STA=0; 


char Rx_Counter=0,Rx_Buffer[200];     //接收缓冲,最大USART_REC_LEN个字节.
UART_HandleTypeDef UART1_Handler; //UART句柄
UART_HandleTypeDef UART2_Handler; //UART句柄
UART_HandleTypeDef UART3_Handler; //UART句柄


uint8_t  flag=0;
volatile uint16_t rx_head = 0;
volatile uint16_t rx_tail = 0;
volatile bool data_ready = false;

DMA_HandleTypeDef DMA1_TX_Handler;

__attribute__((section(".dma_buffer"))) uint8_t dma_rx_buffer[USART3_REC_LEN]; // DMA专用缓冲区

#define TX_BUF_SIZE 1024  // 发送缓冲区大小
uint8_t uart3_tx_buf[TX_BUF_SIZE];  // 发送缓冲区
volatile uint16_t tx_write_pos = 0; // 缓冲区写入位置
volatile uint16_t tx_read_pos = 0;  // 缓冲区读取位置
volatile uint8_t tx_sending = 0;    // 发送状态标志（0=空闲，1=发送中）
uint8_t uart2_tx_buf[TX_BUF_SIZE];  // 发送缓冲区

// 添加在全局变量区域
typedef struct {
    uint8_t buffer[256];        // 发送缓冲区
    volatile uint16_t head;     // 缓冲区写入指针
    volatile uint16_t tail;     // 缓冲区读取指针
    volatile bool is_busy;       // 发送状态标志
} uart_tx_buffer_t;

uart_tx_buffer_t usart3_tx_buf = {0};  // 实例化

// 添加全局发送缓冲区

volatile uint8_t tx_buffer[TX_BUF_SIZE];
volatile uint16_t tx_head = 0, tx_tail = 0;


/**
 * @brief	初始化串口1函数
 *
 * @param	bound	串口波特率
 */
void uart_init(u32 bound)
{

		
	  UART1_Handler.Instance = USART1;					  //USART1
    UART1_Handler.Init.BaudRate = bound;				  //波特率
    UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B; //字长为8位数据格式
    UART1_Handler.Init.StopBits = UART_STOPBITS_1;	  //一个停止位
    UART1_Handler.Init.Parity = UART_PARITY_NONE;		  //无奇偶校验位
    UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //无硬件流控
    UART1_Handler.Init.Mode = UART_MODE_RX;		  //收发模式
    HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1	
	
    __HAL_UART_ENABLE_IT(&UART1_Handler, UART_IT_RXNE); //开启接收中断
    HAL_NVIC_EnableIRQ(USART1_IRQn);					//使能USART1中断通道
    HAL_NVIC_SetPriority(USART1_IRQn, 3, 3);				//抢占优先级3，子优先级3
		  CLR_Buf();//清空缓存
}

/**
 * @brief	初始化串口2函数
 *
 * @param	bound	串口波特率
 *
 * @return  void
 */
void uart2_init(u32 bound)
{
    //UART 初始化设置
    UART2_Handler.Instance = USART2;					  //USART1
    UART2_Handler.Init.BaudRate = bound;				  //波特率
    UART2_Handler.Init.WordLength = UART_WORDLENGTH_8B; //字长为8位数据格式
    UART2_Handler.Init.StopBits = UART_STOPBITS_1;	  //一个停止位
    UART2_Handler.Init.Parity = UART_PARITY_NONE;		  //无奇偶校验位
    UART2_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //无硬件流控
    UART2_Handler.Init.Mode = UART_MODE_TX_RX;		  //收发模式
    HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()会使能UART1

    __HAL_UART_ENABLE_IT(&UART2_Handler, UART_IT_RXNE); //开启接收中断
    HAL_NVIC_EnableIRQ(USART2_IRQn);					//使能USART1中断通道
    HAL_NVIC_SetPriority(USART2_IRQn, 3, 1);				//抢占优先级3，子优先级3
}
/**
 * @brief	初始化串口3函数
 *
 * @param	bound	串口波特率
 *
 * @return  void
 */
void uart3_init(u32 bound)
{
    //UART 初始化设置
    UART3_Handler.Instance = USART3;					  //USART3
    UART3_Handler.Init.BaudRate = bound;				  //波特率
    UART3_Handler.Init.WordLength = UART_WORDLENGTH_8B; //字长为8位数据格式
    UART3_Handler.Init.StopBits = UART_STOPBITS_1;	  //一个停止位
    UART3_Handler.Init.Parity = UART_PARITY_NONE;		  //无奇偶校验位
    UART3_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //无硬件流控
    UART3_Handler.Init.Mode = UART_MODE_TX_RX;		  //收发模式
		UART3_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    UART3_Handler.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    UART3_Handler.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&UART3_Handler);					    //HAL_UART_Init()会使能UART1

    __HAL_UART_ENABLE_IT(&UART3_Handler, UART_IT_RXNE); //开启接收中断
    HAL_NVIC_EnableIRQ(USART3_IRQn);					//使能USART1中断通道
    HAL_NVIC_SetPriority(USART3_IRQn, 3, 0);				//抢占优先级3，子优先级3

}

/**
 * @brief	HAL库串口底层初始化，时钟使能，引脚配置，中断配置
 *
 * @param	huart	串口句柄
 *
 * @return  void
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_Initure;

    if(huart->Instance == USART1) //如果是串口1，进行串口1 MSP初始化
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();				//使能GPIOA时钟
        __HAL_RCC_USART1_CLK_ENABLE();				//使能USART1时钟

        GPIO_Initure.Pin = GPIO_PIN_9;				//PA9
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;		//复用推挽输出
        GPIO_Initure.Pull = GPIO_PULLUP;			//上拉
        GPIO_Initure.Speed = GPIO_SPEED_FAST;		//高速
        GPIO_Initure.Alternate = GPIO_AF7_USART1;	//复用为USART1
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);	   	//初始化PA9

        GPIO_Initure.Pin = GPIO_PIN_10;				//PA10
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);	   	//初始化PA10
    }
		 if(huart->Instance == USART2) //如果是串口2，进行串口2 MSP初始化
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();				//使能GPIOA时钟
        __HAL_RCC_USART2_CLK_ENABLE();				//使能USART2时钟

        GPIO_Initure.Pin = GPIO_PIN_2;				//PA2
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;		//复用推挽输出
        GPIO_Initure.Pull = GPIO_PULLUP;			//上拉
        GPIO_Initure.Speed = GPIO_SPEED_FAST;		//高速
        GPIO_Initure.Alternate = GPIO_AF7_USART2;	//复用为USART2
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);	   	//初始化PA2

        GPIO_Initure.Pin = GPIO_PIN_3;				//PA3
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);	   	//初始化PA3
    }
		if(huart->Instance == USART3) //如果是串口2，进行串口2 MSP初始化
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();				//使能GPIOA时钟
        __HAL_RCC_USART3_CLK_ENABLE();				//使能USART2时钟

        GPIO_Initure.Pin = GPIO_PIN_10;				//PA2
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;		//复用推挽输出
        GPIO_Initure.Pull = GPIO_PULLUP;			//上拉
        GPIO_Initure.Speed = GPIO_SPEED_FAST;		//高速
        GPIO_Initure.Alternate = GPIO_AF7_USART3;	//复用为USART2
        HAL_GPIO_Init(GPIOB, &GPIO_Initure);	   	//初始化PA2

        GPIO_Initure.Pin = GPIO_PIN_11;				//PA3
        HAL_GPIO_Init(GPIOB, &GPIO_Initure);	   	//初始化PA3
    }

}

/**
 * @brief	串口1中断服务程序
 *
 * @remark	下面代码我们直接把中断控制逻辑写在中断服务函数内部
 * 			说明：采用HAL库处理逻辑，效率不高。
 *
 * @param   void
 *__disable_irq(); 
 * @return  void__enable_irq();
 */
void USART1_IRQHandler(void)
{
    u8 Res;
		if(__HAL_UART_GET_FLAG(&UART1_Handler, UART_FLAG_RXNE) != RESET) //接收中断
		{
			 HAL_UART_Receive(&UART1_Handler, &Res, 1, 0);
			if(Res == '$')
			{
				point1 = 0;	
			}
				

				USART_RX_BUF[point1++] = Res;

			if(USART_RX_BUF[0] == '$' && USART_RX_BUF[4] == 'M' && USART_RX_BUF[5] == 'C')			//确定是否收到"GPRMC/GNRMC"这一帧数据
			{
				if(Res == '\n')									   
				{
					memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);      //清空
					memcpy(Save_Data.GPS_Buffer, USART_RX_BUF, point1); 	//保存数据
					Save_Data.isGetData = true;
					point1 = 0;
					memset(USART_RX_BUF, 0, USART1_REC_LEN);      //清空				
				}	
						
			}
			
			if(point1 >= USART1_REC_LEN)
			{
				point1 = USART1_REC_LEN;
			}	
	
		}
	if(point1 >= USART1_REC_LEN)
	{
		point1 = USART1_REC_LEN;
	}

	 		USART_RX_STA|=0x8000;	//接收完成了
 		if((USART_RX_STA&0x8000)==0)//接收未完成
 		{
 			if(USART_RX_STA&0x4000)//接收到了0x0d
 			{
 				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
 				else USART_RX_STA|=0x8000;	//接收完成了 			//bit31表明是否接收到0x0a(\n)
 			}
 			else //还没收到0X0D
 			{	
 				if(Res==0x0d)USART_RX_STA|=0x4000;						//bit30表明是否接收到0x0d(\r)
 				else
 				{
 					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
 					USART_RX_STA++;
 					if(USART_RX_STA>(USART1_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
 				}		 
 			}
		}
    HAL_UART_IRQHandler(&UART1_Handler);
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{

   if (huart->Instance == USART2) {
       
        tx_read_pos = (tx_read_pos + huart->TxXferSize) % TX_BUF_SIZE;
        if (tx_read_pos != tx_write_pos)
		{
            uint16_t len = (tx_write_pos >= tx_read_pos) ? 
                         (tx_write_pos - tx_read_pos) : 
                         (TX_BUF_SIZE - tx_read_pos);
	
        } 
		else 
		{ 	
				__disable_irq();
			   memset(uart2_tx_buf, 0, TX_BUF_SIZE);
			   tx_write_pos = 0;
			   tx_read_pos = 0;
			   tx_sending = 0;  // 重置发送状态
			   __enable_irq();        
		}	
   }
}

void ClearRxBuffer(uint8_t *buf, uint16_t len) {
    for(uint16_t i=0; i<len; i++) {
        buf[i] = 0;
    }
}

void USART2_IRQHandler(void) {
    uint8_t Res;
    
    if(__HAL_UART_GET_FLAG(&UART2_Handler, UART_FLAG_RXNE) != RESET) {
        HAL_UART_Receive(&UART2_Handler, &Res, 1, 100);
        

        if(USART2_RX_STA >= RX_BUF_SIZE) {
            USART2_RX_STA = 0;
        }
        
        Rx_Buffer[USART2_RX_STA] = Res;
        USART2_RX_STA++;
        

        if(USART2_RX_STA >= 3) {
					
   

					if(Rx_Buffer[USART2_RX_STA-3] == '1' && 
               Rx_Buffer[USART2_RX_STA-2] == '1' &&
               Rx_Buffer[USART2_RX_STA-1] == '0') {
                flag = 1;

                ClearRxBuffer(&Rx_Buffer[USART2_RX_STA-3], 3);

            }

            else if(Rx_Buffer[USART2_RX_STA-3] == '0' && 
                    Rx_Buffer[USART2_RX_STA-2] == '0' &&
                    Rx_Buffer[USART2_RX_STA-1] == '1') {
                flag = 0;
                ClearRxBuffer(&Rx_Buffer[USART2_RX_STA-3], 3);

            }
        }
    }
    

    if (__HAL_UART_GET_FLAG(&UART2_Handler, UART_FLAG_TXE) && 
        __HAL_UART_GET_IT_SOURCE(&UART2_Handler, UART_IT_TXE)) {
        
        if (tx_head != tx_tail) {
            USART2->TDR = tx_buffer[tx_tail];
            tx_tail = (tx_tail + 1) % TX_BUF_SIZE;
        } else {
            __HAL_UART_DISABLE_IT(&UART2_Handler, UART_IT_TXE);
        }
        __HAL_UART_CLEAR_FLAG(&UART2_Handler, UART_FLAG_TXE);
    }
    
    HAL_UART_IRQHandler(&UART2_Handler);
}

void USART3_IRQHandler(void)
{
	  if (__HAL_UART_GET_FLAG(&UART3_Handler, UART_FLAG_ORE)) {
        __HAL_UART_CLEAR_FLAG(&UART3_Handler, UART_FLAG_ORE);
        volatile uint8_t temp = UART3_Handler.Instance->RDR; // 必须读取DR以清除ORE
    }
			
   if (__HAL_UART_GET_FLAG(&UART3_Handler, UART_FLAG_RXNE)) {
        uint8_t Res = (uint8_t)(UART3_Handler.Instance->RDR);
        __HAL_UART_CLEAR_FLAG(&UART3_Handler, UART_FLAG_RXNE);
		 
		 __disable_irq(); 
		uint16_t next_pos = (tx_write_pos + 1) % TX_BUF_SIZE;
        if (next_pos != tx_read_pos) {
            uart3_tx_buf[tx_write_pos] = Res;
            tx_write_pos = next_pos;
        } else {
            // 缓冲区满时丢弃最旧数据（可选逻辑）
            tx_read_pos = (tx_read_pos + 1) % TX_BUF_SIZE; 
            uart3_tx_buf[tx_write_pos] = Res;
            tx_write_pos = next_pos;
        }
		__enable_irq();		 
		 
        // 触发发送（非阻塞）
        if (!tx_sending) {
            __disable_irq();
            tx_sending = 1;
            uint16_t len = (tx_write_pos >= tx_read_pos) ? 
                         (tx_write_pos - tx_read_pos) : 
                         (TX_BUF_SIZE - tx_read_pos);			
            __enable_irq();
			
        }		 
        // 处理接收数据帧（简化逻辑）
        if (Res == 'v') 
        {
			memset(USART3_RX_BUF, 0, USART3_REC_LEN);  // 清空接收缓冲区
            point2 = 0;
            USART3_RX_STA = 0;  // 重置接收状态
					num1=1;
				}
			if (num1==1) 
			{				
        if (point2 < USART3_REC_LEN - 1)
        {
            USART3_RX_BUF[point2++] = Res;
        }
        // 检测帧结束符'\n'
        if (Res == '\n' ) 
        {
            USART3_RX_STA |= 0x8000;  // 标记帧接收完成
					num1=0;
        }
			}
			
				if(Res == 'g')
				{
			memset(USARTg_RX_BUF, 0, USARTg_REC_LEN);  // 清空接收缓冲区
			
            point3 = 0;
            USARTg_RX_STA = 0;  // 重置接收状态
						num2=1;
				}
			if (num2==1) 
			{
        if (point3 < USARTg_REC_LEN - 1 )
        {
            USARTg_RX_BUF[point3++] = Res;
        }
        // 检测帧结束符'\n'
        if (Res == '\n') 
        {		
            USARTg_RX_STA |= 0x4000;  // 标记帧接收完成
						num2=0;
        }	
			}
			
				if(Res == 'y' )
				{
			memset(USARTy_RX_BUF, 0, USARTy_REC_LEN);  // 清空接收缓冲
			
            point4 = 0;
            USARTy_RX_STA = 0;  // 重置接收状态
					  num3=1;
				}
			if (num3 == 1) 
			{
        if (point4 < USARTy_REC_LEN - 1 )
        {
            USARTy_RX_BUF[point4++] = Res;
        }
        // 检测帧结束符'\n'
        if (Res == '\n') 
        {		
            USARTy_RX_STA |= 0x2000;  // 标记帧接收完成
						num3=0;
        }
			}				
		}
   HAL_UART_IRQHandler(&UART3_Handler);
								
}


bool Uart2_SendNonBlocking(const uint8_t *data, uint16_t len) {
    uint16_t bytes_added = 0;
    uint32_t primask = __get_PRIMASK();
    
    __disable_irq();
    for (uint16_t i = 0; i < len; i++) {
        uint16_t next_head = (tx_head + 1) % TX_BUF_SIZE;
        if (next_head == tx_tail) break;
        
        tx_buffer[tx_head] = data[i];
        tx_head = next_head;
        bytes_added++;
    }
    
    if (bytes_added > 0) {
        __HAL_UART_ENABLE_IT(&UART2_Handler, UART_IT_TXE); // 触发发送
    }
    __set_PRIMASK(primask);
    return (bytes_added == len);
}

void Uart3_SendStr(char*SendBuf)//串口3打印数据
{
	while(*SendBuf)
	{
		while((USART3->ISR & 0X40) == 0); //循环发送,直到发送完毕

		USART3->TDR = (u8) *SendBuf;
    SendBuf++;
	}
}


u8 Hand(char *a)                   // 串口命令识别函数
{ 
    if(strstr(USART_RX_BUF,a)!=NULL)
	    return 1;
	else
		return 0;
}

void CLR_Buf(void)                           // 串口缓存清理
{
	memset(USART_RX_BUF, 0, USART1_REC_LEN);      //清空
  point1 = 0;                    
}

void clrStruct()
{
	Save_Data.isGetData = false;
	Save_Data.isParseData = false;
	Save_Data.isUsefull = false;
	Save_Data.isGetAT = false;
	memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);      //清空
	memset(Save_Data.UTCTime, 0, UTCTime_Length);
	memset(Save_Data.latitude, 0, latitude_Length);
	memset(Save_Data.N_S, 0, N_S_Length);
	memset(Save_Data.longitude, 0, longitude_Length);
	memset(Save_Data.E_W, 0, E_W_Length);
	memset(Save_Data.temp, 0, 10);
	memset(Save_Data.data, 0, 5);
	
}

#endif



