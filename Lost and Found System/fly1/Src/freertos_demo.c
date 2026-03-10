#include "freertos_demo.h"

#include "FreeRTOS.h"
#include "task.h"
/* 需要用到的其他头文件 */
#include "buzzer.h"
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "ms5611.h"
#include "mpu6050.h"
#include "kalman.h"
#include "stdio.h"
#include "string.h"
#include "LED.h"
#include <math.h>
#include "exti.h"
#include "M5311.h"
#include "timer.h"
#include "sys.h"
#include "bh1750.h"
#include <stdbool.h>
#include "NanoEdgeAI.h"
#include "dma.h"
#include "usart.h"
#include "delay.h"
/* 启动任务的配置 */
#define START_TASK_STACK 256
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handle;
void start_task(void *pvParameters);

/* 任务1的配置 */
#define TASK1_STACK 256
#define TASK1_PRIORITY 3
TaskHandle_t task1_handle;
void task1(void *pvParameters);

/* 任务2的配置 */
#define TASK2_STACK 256
#define TASK2_PRIORITY 5
TaskHandle_t task2_handle;
void task2(void *pvParameters);

/* 任务3的配置 */
#define TASK3_STACK 256
#define TASK3_PRIORITY 4
TaskHandle_t task3_handle;
void task3(void *pvParameters);

/**
 * @description: 启动FreeRTOS
 * @return {*}
 */
void freertos_start(void)
{
    /* 1.创建一个启动任务 */
    xTaskCreate((TaskFunction_t)start_task,               // 任务函数的地址
                (char *)"start_task",                     // 任务名字符串
                (configSTACK_DEPTH_TYPE)START_TASK_STACK, // 任务栈大小，默认最小128，单位4字节
                (void *)NULL,                             // 传递给任务的参数
                (UBaseType_t)START_TASK_PRIORITY,         // 任务的优先级
                (TaskHandle_t *)&start_task_handle);      // 任务句柄的地址

    /* 2.启动调度器:会自动创建空闲任务 */
    vTaskStartScheduler();
}

/**
 * @description: 启动任务：用来创建其他Task
 * @param {void} *pvParameters
 * @return {*}
 */
void start_task(void *pvParameters)
{
    /* 进入临界区:保护临界区里的代码不会被打断 */
    taskENTER_CRITICAL();

    /* 创建3个任务 */
    xTaskCreate((TaskFunction_t)task1,
                (char *)"task1",
                (configSTACK_DEPTH_TYPE)TASK1_STACK,
                (void *)NULL,
                (UBaseType_t)TASK1_PRIORITY,
                (TaskHandle_t *)&task1_handle);
    xTaskCreate((TaskFunction_t)task2,
                (char *)"task2",
                (configSTACK_DEPTH_TYPE)TASK2_STACK,
                (void *)NULL,
                (UBaseType_t)TASK2_PRIORITY,
                (TaskHandle_t *)&task2_handle);
    xTaskCreate((TaskFunction_t)task3,
                (char *)"task3",
                (configSTACK_DEPTH_TYPE)TASK3_STACK,
                (void *)NULL,
                (UBaseType_t)TASK3_PRIORITY,
                (TaskHandle_t *)&task3_handle);	
				
    /* 启动任务只需要执行一次即可，用完就删除自己 */
    vTaskDelete(NULL);

    /* 退出临界区 */
    taskEXIT_CRITICAL();
}


/* 传感器数据获取 */
void task1(void *pvParameters)
{
	char str2[100];

    while (1)
    {

	  get_Altitude();	  
	  get_LightData_Hex();
		Angle_Calcu();
		
			
		vTaskDelay(200);
    }
}



/* GPS数据解析函数 */
void task2(void *pvParameters)
{
	char str[10];
	char str2[100];

   while (1)
    {	
			
			if (USART_RX_STA & 0x8000) 
			{ 	
				// 解析GPS数据
				parseGpsBuffer();
	
				// 重置状态
				USART_RX_STA = 0;
				point1 = 0;		
				
			}			
			
      vTaskDelay(800);
    }
}



/* 数据发送函数 */

void task3(void *pvParameters)
{	

  char str [100];
	char str1 [100];
	char str2[100];
	char str3 [100];
	char str4[100];
	char str5 [100];
	
	while (1)
    {		

		AIwarning( );						
			
		printGpsBuffer();	
	  snprintf(str, sizeof(str),"x:%.3f \r\n",Angle_X_Final);
		Uart2_SendNonBlocking((uint8_t*)str, strlen(str));
		
		snprintf(str, sizeof(str),"y:%.3f \r\n",Angle_Y_Final);
		Uart2_SendNonBlocking((uint8_t*)str, strlen(str));
		
		snprintf(str, sizeof(str),"z:%.3f \r\n",Angle_Z_Final);
		Uart2_SendNonBlocking((uint8_t*)str, strlen(str));

			
		snprintf(str2, sizeof(str2),"Times: %d \r\n",times);
		Uart2_SendNonBlocking((uint8_t*)str2, strlen(str2));	

		snprintf(str2, sizeof(str2),"altitude:%f \r\n",Altitude);
		Uart2_SendNonBlocking((uint8_t*)str2, strlen(str2));		
		
		snprintf(str2, sizeof(str2),"light_lx:%d lx\r\n",LightData_Hex);
		Uart2_SendNonBlocking((uint8_t*)str2, strlen(str2));

		if ((USART3_RX_STA & 0x8000) &&(USART3_RX_BUF[0]=='v' && USART3_RX_BUF[2]=='l' && USART3_RX_BUF[3]=='t' && USART3_RX_BUF[6]=='e'))
		{ 
			snprintf(str3, sizeof(str3),"%.*s ",point2, USART3_RX_BUF);
			Uart2_SendNonBlocking((uint8_t*)str3, strlen(str3));
				
		// 重置状态
			USART3_RX_STA = 0;			
			point2 = 0;	
	}else
		{
			snprintf(str3, sizeof(str3),"voltage=0.0 \r\n");
			Uart2_SendNonBlocking((uint8_t*)str3, strlen(str3));	
			USART3_RX_STA = 0;
			point2 = 0;				
		}

		
		if ((USARTg_RX_STA & 0x4000)&& ( USARTg_RX_BUF[0]=='g' && USARTg_RX_BUF[3]=='u' && USARTg_RX_BUF[4]=='n' && USARTg_RX_BUF[10]=='d'))
		{ 
			snprintf(str4, sizeof(str4),"%.*s ",point3, USARTg_RX_BUF);
			Uart2_SendNonBlocking((uint8_t*)str4, strlen(str4));
				
			// 重置状态
			USARTg_RX_STA = 0;			
			point3 = 0;	
	}else
		{
			snprintf(str4, sizeof(str4),"groundspeed=0.0 m/s\r\n");
			Uart2_SendNonBlocking((uint8_t*)str4, strlen(str4));	
			USARTg_RX_STA = 0;
			point3 = 0;			
		}

		if ((USARTy_RX_STA & 0x2000)&&( USARTy_RX_BUF[0]=='y' && USARTy_RX_BUF[1]=='a' && USARTy_RX_BUF[2]=='w'))
		{ 

			snprintf(str5, sizeof(str5),"%.*s ",point4, USARTy_RX_BUF);
			Uart2_SendNonBlocking((uint8_t*)str5, strlen(str5));
				
			// 重置状态
			USARTy_RX_STA = 0;			
			point4 = 0;	
	}else
		{
			snprintf(str5, sizeof(str5),"yaw=null \r\n");
			Uart2_SendNonBlocking((uint8_t*)str5, strlen(str5));	
			USARTy_RX_STA = 0;			
			point4 = 0;		
		}		

		if(flag==1)
		{
			if(LightData_Hex<3)
			{
				LED_R (1);
			}
			else
			{
				BUZZER(1);		
			}
		}else if(flag==0)	
		{
			LED_R (0);
			BUZZER(0);				
		}		
		
    vTaskDelay(600);
    }
}
