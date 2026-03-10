#include "timer.h"
#include "led.h"
#include "Buzzer.h"
#include "M5311.h"
#include "kalman.h"
#include "mpu6050.h"
#include "ms5611.h"
#include "sys.h"
#include "bh1750.h"
#include "NanoEdgeAI.h"
char *DataString;
char *DataStringNext;
char *ATString;
char *ATStringNext;
extern char str1[4];
extern char str2[40];
extern float a;
u8 i = 0;
enum neai_state error_code;

TIM_HandleTypeDef TIM16_Handler;      //定时器句柄
TIM_HandleTypeDef TIM15_Handler;

//int16_t AX, AY, AZ, GX, GY, GZ;
//float accx, accy, accz;

/**
 * @brief	通用定时器3中断初始化函数
 *			定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
 *			Ft=定时器工作频率,单位:Mhz
 *			这里使用的是定时器3!(定时器3挂在APB1上，时钟为HCLK:80Mhz)
 *
 * @param   arr		自动重装值。
 * @param   psc		时钟预分频数
 *
 * @return  void
 */
void TIM16_Init(u16 arr, u16 psc)
{
    TIM16_Handler.Instance = TIM16;                          	//通用定时器3
    TIM16_Handler.Init.Prescaler = psc;                     	//分频系数
    TIM16_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;    	//向上计数器
    TIM16_Handler.Init.Period = arr;                        	//自动装载值
    TIM16_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM16_Handler);

    HAL_TIM_Base_Start_IT(&TIM16_Handler);					//使能定时器3和定时器3更新中断：TIM_IT_UPDATE
}

void TIM15_Init(u16 arr, u16 psc)
{
    TIM15_Handler.Instance = TIM15;                          	//通用定时器3
    TIM15_Handler.Init.Prescaler = psc;                     	//分频系数
    TIM15_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;    	//向上计数器
    TIM15_Handler.Init.Period = arr;                        	//自动装载值
    TIM15_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM15_Handler);

    HAL_TIM_Base_Start_IT(&TIM15_Handler);					//使能定时器3和定时器3更新中断：TIM_IT_UPDATE
}
/**
 * @brief	定时器底册驱动，开启时钟，设置中断优先级
 *			此函数会被HAL_TIM_Base_Init()函数调用
 *
 * @param   htim	定时器句柄
 *
 * @return  void
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM16)
    {
        __HAL_RCC_TIM16_CLK_ENABLE();            //使能TIM3时钟
        HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 5   , 0);  //设置中断优先级，抢占优先级1，子优先级3
        HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);          //开启ITM3中断
    }
    if(htim->Instance == TIM15)
    {
        __HAL_RCC_TIM15_CLK_ENABLE();            //使能TIM3时钟
        HAL_NVIC_SetPriority( TIM1_BRK_TIM15_IRQn , 5   , 1);  //设置中断优先级，抢占优先级1，子优先级3
        HAL_NVIC_EnableIRQ( TIM1_BRK_TIM15_IRQn );          //开启ITM3中断
    }
}

/**
 * @brief	定时器3中断服务函数
 *
 * @param   void
 *
 * @return  void
 */
void TIM1_UP_TIM16_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM16_Handler);
}
void TIM1_UP_TIM15_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM15_Handler);
}
/**
 * @brief	回调函数，定时器中断服务函数调用
 *
 * @param   htim	定时器句柄
 *
 * @return  void
 */
u8 delay_30s=0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	
	
	
//	double data1=29.323456,data2=115.323456;
    if(htim == (&TIM16_Handler))
    {
//			  FLAG=1;	
//				if(delay_30s==30)
//				{	
//					get_Altitude(); 				
//					printf("AT+MIPLNOTIFY=0,0,3200,0,5751,1,5,\"%f\",0,0\r\n",Altitude);				
//				}			
//				if(delay_30s==40)
//				{		 				
//					printf("AT+MIPLNOTIFY=0,0,3200,0,5750,1,10,\"%f\",0,0\r\n",Angle_X_Final);
////						uart_init(9600);	
//				}
//				if(delay_30s==50)
//				{		 				
//					printf("AT+MIPLNOTIFY=0,0,3200,0,5750,1,10,\"%f\",0,0\r\n",Angle_Y_Final);          			
//				}
//				if(delay_30s==51)
//				{		
//						delay_30s=0;
//						Clear_Buffer();					
//				}
//				delay_30s++;


//				DataString=strstr(Rx_Buffer,",3200,0,5750,1,");//有读请求产生,这里根据不同的请求发送不同的指令需求
//				if(DataString)
//				{
//					if ((DataStringNext = strstr(DataString+18, ",0")) != NULL)
//					{
//							for (i = 0 ; i <= 8 ; i++)
//							{
//								if (i == 0)
//								{
//									if ((ATString = strstr(Rx_Buffer, "+MIPLWRITE:0,")) == NULL);		
//								}							
//								else
//								{
//									ATString++;
//									if ((ATStringNext = strstr(ATString, ",")) != NULL)
//									{
//										switch(i)
//										{
//											case 1:break;
//											case 2:memcpy(Save_Data.temp, ATString, ATStringNext - ATString);break;	//获取UTC时间
//											case 3:break;
//											case 4:break;
//											case 5:break;
//											case 6:break;
//											case 7:break;
//											case 8:memcpy(Save_Data.data, ATString, ATStringNext - ATString);break;	//获取E/W

//											default:break;
//										}

//										ATString = ATStringNext;


//									}
//								}


//							 }
//							Save_Data.isGetAT = true;
////							Uart3_SendStr(Rx_Buffer);
//							Clear_Buffer();

//					}


//				}
//				if(Save_Data.isGetAT)
//				{
//						printf("AT+MIPLWRITERSP=0,%s,2\r\n",Save_Data.temp);//回复请求，将数据传输上去
//						if(Save_Data.data[0] == '1')
//						{
//							BUZZER(1);
////							LED_R (1);
//						}						
//						if(Save_Data.data[0] == '0')
//						{
////							LED_R (0);
//							BUZZER(0);
//						}
//						if(Save_Data.data[0] == '2')
//						{
//							LED_R (0);
//						}
//						Save_Data.isGetAT = false;
//				}
//		get_Altitude();

//	  
//	  wt( );
// get_LightData_Hex();
//	  
//	  Angle_Calcu();	
//	  
//	MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);  
//	
//	float AX_m_s2 = AX / 1420.0 * 9.8;
//	float AY_m_s2 = AY / 1420.0 * 9.8;
//	float AZ_m_s2 = AZ / 1420.0 * 9.8;



//	snprintf(str2, sizeof(str2),"X加速度: %.2f \r\n",AX_m_s2);
//    Uart3_SendNonBlocking((uint8_t*)str2, strlen(str2));

//	snprintf(str2, sizeof(str2),"Y加速度: %.2f \r\n",AY_m_s2);
//    Uart3_SendNonBlocking((uint8_t*)str2, strlen(str2));
//	
//	snprintf(str2, sizeof(str2),"Z加速度: %.2f \r\n",AZ_m_s2);
//    Uart3_SendNonBlocking((uint8_t*)str2, strlen(str2));	

//	snprintf(str2, sizeof(str2),"x: %f \r\n",Angle_X_Final);
//    Uart3_SendNonBlocking((uint8_t*)str2, strlen(str2));
//	
//	snprintf(str2, sizeof(str2),"y: %f \r\n",Angle_Y_Final);
//    Uart3_SendNonBlocking((uint8_t*)str2, strlen(str2));
//		
//	snprintf(str2, sizeof(str2),"Times: %d \r\n",times);
//    Uart3_SendNonBlocking((uint8_t*)str2, strlen(str2));	
//	
//	snprintf(str2, sizeof(str2),"海拔:%f \r\n",Altitude);
//    Uart3_SendNonBlocking((uint8_t*)str2, strlen(str2));		
//	
//	snprintf(str2, sizeof(str2),"光照强度:%d lx\r\n",LightData_Hex);
//    Uart3_SendNonBlocking((uint8_t*)str2, strlen(str2));	
	



    }
	
    if(htim == (&TIM15_Handler))
		{
//				if(delay_30s==30)
//				{	
//					get_Altitude(); 				
//					printf("AT+MIPLNOTIFY=0,0,3200,0,5751,1,5,\"%f\",0,0\r\n",Altitude);				
//				}			
//				if(delay_30s==40)
//				{		 				
//					printf("AT+MIPLNOTIFY=0,0,3200,0,5750,1,10,\"%f\",0,0\r\n",Angle_X_Final);
//				}
//				if(delay_30s==50)
//				{		 				
//					printf("AT+MIPLNOTIFY=0,0,3200,0,5750,1,10,\"%f\",0,0\r\n",Angle_Y_Final);          			
//				}
//				if(delay_30s==51)
//				{		
//						delay_30s=0;
//						Clear_Buffer();					
//				}
//				delay_30s++;				
		}
}
