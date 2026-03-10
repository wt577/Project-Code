#include "uart.h"
#include "mavlink.h"
#include "common.h"

#define TX_BUF_SIZE 1024  // 发送缓冲区大小
#define RX_BUF_SIZE 1024  // 发送缓冲区大小
uint8_t uart3_tx_buf[TX_BUF_SIZE];  // 发送缓冲区
volatile uint16_t tx_write_pos = 0; // 缓冲区写入位置
volatile uint16_t tx_read_pos = 0;  // 缓冲区读取位置
volatile uint8_t tx_sending = 0;    // 发送状态标志（0=空闲，1=发送中）
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

#if 1
#pragma import(__use_no_semihosting)   
#define TX_BUF_SIZE 1024  // 发送缓冲区大小
volatile uint8_t tx_buffer[TX_BUF_SIZE];
volatile uint16_t tx_head = 0, tx_tail = 0;
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{
		while((USART2->ISR & 0X40) == 0); //循环发送,直到发送完毕
		USART2->TDR = (u8) ch;
    return ch;
}
#endif
 
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART3_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART2_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

UART_HandleTypeDef UART1_Handler; //UART句柄
UART_HandleTypeDef UART2_Handler; //UART句柄
UART_HandleTypeDef UART3_Handler; //UART句柄

u16 USART_RX_STA=0;       //接收状态标记	
u16 USART2_RX_STA=0;       //接收状态标记
u16 USART3_RX_STA=0;       //接收状态标记
char Rx_Counter=0,Rx_Buffer[200];     //接收缓冲,最大USART_REC_LEN个字节.
u8 mavlink=0;
u8 mavlink1=0;
u8 mavlink2=0;


	mavlink_message_t msg; 
  mavlink_status_t status;
	mavlink_attitude_t attitude; 
  mavlink_heartbeat_t heartbeat;
  mavlink_sys_status_t sys_status;
	mavlink_set_mode_t set_mode;
	mavlink_battery_status_t battery_status;
	mavlink_esc_status_t esc_status;
	mavlink_vfr_hud_t  vfr_hud;
	mavlink_highres_imu_t highres_imu;
	
void uart_init(u32 bound)
{		
	  UART1_Handler.Instance = USART1;					  //USART1
    UART1_Handler.Init.BaudRate = bound;				  //波特率
    UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B; //字长为8位数据格式
    UART1_Handler.Init.StopBits = UART_STOPBITS_1;	  //一个停止位
    UART1_Handler.Init.Parity = UART_PARITY_NONE;		  //无奇偶校验位
    UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //无硬件流控
    UART1_Handler.Init.Mode = UART_MODE_TX_RX;		  //收发模式
		UART1_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1	
	
    __HAL_UART_ENABLE_IT(&UART1_Handler, UART_IT_RXNE); //开启接收中断
    HAL_NVIC_EnableIRQ(USART1_IRQn);					//使能USART1中断通道
    HAL_NVIC_SetPriority(USART1_IRQn, 3, 3);				//抢占优先级3，子优先级3
}
/**
 * @brief	初始化串口2函数
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
		UART1_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()会使能UART1

    __HAL_UART_ENABLE_IT(&UART2_Handler, UART_IT_RXNE); //开启接收中断
    HAL_NVIC_EnableIRQ(USART2_IRQn);					//使能USART1中断通道
    HAL_NVIC_SetPriority(USART2_IRQn, 3, 4);				//抢占优先级3，子优先级3
}
/**
 * @brief	初始化串口3函数
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


void USART1_IRQHandler(void)               	//串口1中断服务程序
{
	uint8_t Res,i,c;
	char str[100];
	char str1[100];
	char str2[100];
	char str3[100];
	char str4[100];
	
	if(__HAL_UART_GET_FLAG(&UART1_Handler, UART_FLAG_RXNE) != RESET)
	{
		HAL_UART_Receive(&UART1_Handler, &Res, 1, 10);	//读取接收到的数据
		
		USART_RX_BUF[USART_RX_STA]=Res;	
		if(mavlink_parse_char(MAVLINK_COMM_0, USART_RX_BUF[USART_RX_STA], &msg, &status))
		{				

//			if(msg.msgid==MAVLINK_MSG_ID_BATTERY_STATUS)//电量
//			{
//				 	mavlink++;
//					mavlink_msg_battery_status_decode(&msg,&battery_status);
//					float voltage1 = battery_status.voltages[0] / 10000.0f;
//					float voltage2 = battery_status.voltages[1] / 10000.0f;
//					float voltage3 = battery_status.voltages[2] / 10000.0f;

//				
//					if(mavlink>3){
//					snprintf(str2,sizeof(str2),"voltage=%.1f\r\n",voltage1+voltage2+voltage3);
//					Uart3_SendNonBlocking((uint8_t*)str2, strlen(str2));
//					mavlink=0;}
//			}
//			if(msg.msgid == MAVLINK_MSG_ID_BATTERY_STATUS) 
//{
//    mavlink++;
//    mavlink_msg_battery_status_decode(&msg, &battery_status);
//    
//    float total_voltage = battery_status.voltages[0] / 1000.0f; 
//    
//    if(mavlink > 3) {
//        snprintf(str2, sizeof(str2), "voltage=%.1fV\r\n",total_voltage); //total_voltage
//        Uart3_SendNonBlocking((uint8_t*)str2, strlen(str2));
//        mavlink = 0;
//    }
//}
			if(msg.msgid==MAVLINK_MSG_ID_BATTERY_STATUS)//电量
			{
				mavlink++;
				mavlink_msg_battery_status_decode(&msg,&battery_status);
				float voltage1 = (battery_status.voltages[0] > 0 && battery_status.voltages[0] < 65535) ? (battery_status.voltages[0] / 1000.0f) : 0;
				float voltage2 = (battery_status.voltages[1] > 0 && battery_status.voltages[1] < 65535) ? (battery_status.voltages[1] / 1000.0f) : 0;
				float voltage3 = (battery_status.voltages[2] > 0 && battery_status.voltages[2] < 65535) ? (battery_status.voltages[2] / 1000.0f) : 0;
	
				snprintf(str2,sizeof(str2),"voltage=%.1f\r\n",voltage1+voltage2+voltage3);
				Uart3_SendNonBlocking((uint8_t*)str2, strlen(str2));
			}	
		
			if (msg.msgid == MAVLINK_MSG_ID_VFR_HUD)//对地速度
			{
				mavlink1++;
				mavlink_msg_vfr_hud_decode(&msg, &vfr_hud);

				if(mavlink1>0)
				{
			  snprintf(str4,sizeof(str4),"groundspeed=%.1f m/s\r\n", vfr_hud.groundspeed);
				Uart3_SendNonBlocking((uint8_t*)str4, strlen(str4));
				mavlink1=0;
				}
			}

			if (msg.msgid == MAVLINK_MSG_ID_ATTITUDE)//航向角
			{		
				mavlink2++;
				mavlink_msg_attitude_decode(&msg,&attitude);
				if(mavlink2>5)
				{
				snprintf(str3,sizeof(str3),"yaw=%.1f\r\n",attitude.yaw);
				Uart3_SendNonBlocking((uint8_t*)str3, strlen(str3));	
				mavlink2=0;
				}					
			}
		}		
		USART_RX_STA++;
		if(USART_RX_STA>1023)USART_RX_STA=0; 		 
  }
	HAL_UART_IRQHandler(&UART1_Handler);	
}


void USART2_IRQHandler(void)
{
		u8 Res;
		if(__HAL_UART_GET_FLAG(&UART2_Handler, UART_FLAG_RXNE) != RESET) //接收中断
				{
					 HAL_UART_Receive(&UART2_Handler, &Res, 1, 1000);
					 USART2_RX_BUF[USART2_RX_STA]=Res;
					 USART2_RX_STA++;
					if(USART2_RX_STA>1023)USART2_RX_STA=0;
				}

		 if (__HAL_UART_GET_FLAG(&UART2_Handler, UART_FLAG_TXE) && 
        __HAL_UART_GET_IT_SOURCE(&UART2_Handler, UART_IT_TXE)) {
        
        if (tx_head != tx_tail) {
            USART2->TDR = tx_buffer[tx_tail];
            tx_tail = (tx_tail + 1) % TX_BUF_SIZE;
        } else {
            __HAL_UART_DISABLE_IT(&UART2_Handler, UART_IT_TXE); // 缓冲区空时关闭中断
        }
        __HAL_UART_CLEAR_FLAG(&UART2_Handler, UART_FLAG_TXE);
    }
    HAL_UART_IRQHandler(&UART2_Handler);
}

void USART3_IRQHandler(void)                	//串口3中断服务程序
{
 u8 Res;
		if(__HAL_UART_GET_FLAG(&UART3_Handler, UART_FLAG_RXNE) != RESET) //接收中断
				{
					 HAL_UART_Receive(&UART3_Handler, &Res, 1, 1000);
					 USART3_RX_BUF[USART3_RX_STA]=Res;
					 USART3_RX_STA++;
					if(USART3_RX_STA>1023)USART3_RX_STA=0;
				}

		 if (__HAL_UART_GET_FLAG(&UART3_Handler, UART_FLAG_TXE) && 
        __HAL_UART_GET_IT_SOURCE(&UART3_Handler, UART_IT_TXE)) {
        
        if (tx_head != tx_tail) {
            USART3->TDR = tx_buffer[tx_tail];
            tx_tail = (tx_tail + 1) % TX_BUF_SIZE;
        } else {
            __HAL_UART_DISABLE_IT(&UART3_Handler, UART_IT_TXE); // 缓冲区空时关闭中断
        }
        __HAL_UART_CLEAR_FLAG(&UART3_Handler, UART_FLAG_TXE);
    }
					
	  HAL_UART_IRQHandler(&UART3_Handler);	
} 	
	

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{	
   if (huart->Instance == USART3) {
       
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
			   memset(uart3_tx_buf, 0, TX_BUF_SIZE);
			   tx_write_pos = 0;
			   tx_read_pos = 0;
			   tx_sending = 0;  // 重置发送状态
			   __enable_irq();     
		}
		
   }
}


bool Uart3_SendNonBlocking(const uint8_t *data, uint16_t len) {
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
        __HAL_UART_ENABLE_IT(&UART3_Handler, UART_IT_TXE); // 触发发送
    }
    __set_PRIMASK(primask);
    return (bytes_added == len);
}
