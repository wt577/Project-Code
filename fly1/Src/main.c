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
#include "freertos_demo.h"
#include "FreeRTOS.h"
#include "task.h"

/* USER CODE END Includes */
#define LEARNING_ITERATIONS 10


float input_user_buffer[DATA_INPUT_USER * AXIS_NUMBER]; // Buffer of input values

void SystemClock_Config(void);
void errorLog(int num);

double convertToDecimalDegrees(const char *latLon, const char *direction);


char str3[100];
int t=0,times=0;

double latitude, longitude;	
uint16_t AI = 0;
uint8_t similarity = 0;
float a;
extern enum neai_state error_code;


/* 向输入缓冲区input_buffer填充传感器计算的角度数据 */
void fill_buffer(float input_buffer[])
{
	
	memset(input_buffer, 0, sizeof((float *)input_buffer));	
    // 定义一个标志来控制是否停止接收和发送
    bool stopFlag = false;

    for (AI = 0; AI < DATA_INPUT_USER; AI++)
    {
        if (!stopFlag)
        {
            Angle_Calcu();
            input_buffer[AXIS_NUMBER * AI] = Angle_X_Final;
            input_buffer[AXIS_NUMBER * AI + 1] = Angle_Y_Final;
        }
    }

    // 在这里添加停止接收和发送的代码
    stopFlag = true; 
}

int16_t AX, AY, AZ, GX, GY, GZ;

uint16_t count;

char str1[4];
char str2[40];

int main(void)
{
	
	/* 变量 */
	char str[100];


	
		
	HAL_Init();
	SystemClock_Config();
	delay_init(80);
	

	uart2_init(115200);		//初始化串口，波特率为115200
	uart3_init(9600);
	uart_init(9600);
  clrStruct();
	EXTI_Init();
	LED_Init();				//初始化LED
	BUZZER_Init();
	POWER_RESET_Init();							


	MX_I2C1_Init();
  MX_I2C3_Init();
	MX_GPIO_Init();
  MX_SPI1_Init();


  ms5611_init();			/* 传感器初始化 */	
  MPU6050_Init();	
  BH1750_init();	

	Clear_Buffer();
	


					/* 异常检测库 学习 */
					/* 1.蜂鸣器提示开始学习（BUZZER(1)）。
					   2.循环调用fill_buffer填充输入数据到input_user_buffer。
					   3.通过neai_anomalydetection_learn进行模型训练（持续MINIMUM_ITERATION_CALLS_FOR_EFFICIENT_LEARNING次）。 
					*/

	error_code = neai_anomalydetection_init();
	uint8_t similarity = 0;

	if (error_code != NEAI_OK) {  	LED_R (1);
		/* This happens if the library works into a not supported board. */
	}

	/* Learning process ----------------------------------------------------------*/
	
					
	BUZZER(1); 		//开始
	LED_R (1);
	for (uint16_t iteration = 0 ; iteration < MINIMUM_ITERATION_CALLS_FOR_EFFICIENT_LEARNING ; iteration++) {
		fill_buffer(input_user_buffer);
		neai_anomalydetection_learn(input_user_buffer);
		
	}
	
	BUZZER(0);		//结束
	LED_R (0);
#ifdef __cplusplus  
}  
#endif //__cplusplus 
	
	freertos_start();

  while (1)
  {	
	}
  
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART3
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C3;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void errorLog(int num)
{
	
}


/* 解析GPS模块的信息存储在变量中 */
void parseGpsBuffer()
{
	char *subString;
	char *subStringNext;
	char i = 0;
	if (Save_Data.isGetData)
	{	
		Save_Data.isGetData = false;
		
		for (i = 0 ; i <= 6 ; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
					errorLog(1);	//解析错误
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					char usefullBuffer[2]; 
					switch(i)
					{
            case 1:
								strncpy(Save_Data.UTCTime, subString, subStringNext - subString);
								Save_Data.UTCTime[subStringNext - subString] = '\0';
								break;

						case 2:
								strncpy(usefullBuffer, subString, subStringNext - subString);
								usefullBuffer[subStringNext - subString] = '\0';
								break;

						case 3:
								strncpy(Save_Data.latitude, subString, subStringNext - subString);
								Save_Data.latitude[subStringNext - subString] = '\0';
								break;

						case 4:
								strncpy(Save_Data.N_S, subString, subStringNext - subString);
								Save_Data.N_S[subStringNext - subString] = '\0';
								break;

						case 5:
								strncpy(Save_Data.longitude, subString, subStringNext - subString);
								Save_Data.longitude[subStringNext - subString] = '\0';
								break;

						case 6:
								strncpy(Save_Data.E_W, subString, subStringNext - subString);
								Save_Data.E_W[subStringNext - subString] = '\0';
								break;
						
						default:break;
					}

					subString = subStringNext;
					Save_Data.isParseData = true;
					if(usefullBuffer[0] == 'A')
					Save_Data.isUsefull = true;
					else if(usefullBuffer[0] == 'V')
					Save_Data.isUsefull = false;
				}
				else
				{
					errorLog(2);	//解析错误
				}
			}
		}
	}
}




/*          打印信息         */
void printGpsBuffer()
{
	if (Save_Data.isParseData)
	{
		Save_Data.isParseData = false;
		if(Save_Data.isUsefull)
		{
			latitude = convertToDecimalDegrees(Save_Data.latitude, Save_Data.N_S);
			longitude = convertToDecimalDegrees(Save_Data.longitude, Save_Data.E_W);
			
			snprintf(str2, sizeof(str2),"N:%f,E:%f\r\n",latitude, longitude);
			Uart2_SendNonBlocking((uint8_t*)str2, strlen(str2));
			
			Save_Data.isUsefull = false;
		}
		else
		{
			snprintf(str2, sizeof(str2),"GPS DATA is not usefull!\r\n");
			Uart2_SendNonBlocking((uint8_t*)str2, strlen(str2));			
		}
	}	
		else
		{
			snprintf(str2, sizeof(str2),"N:%f,E:%f\r\n",latitude, longitude);
			Uart2_SendNonBlocking((uint8_t*)str2, strlen(str2));		
		}	
}



double convertToDecimalDegrees(const char *latLon, const char *direction)
{
	char deg[4] = { 0 };
	char *dot, *min;
	int len;
	double dec = 0;
	if ((dot = strchr(latLon, '.')))
	{                                         // decimal point was found
		min = dot - 2;                          // mark the start of minutes 2 chars back
		len = min - latLon;                     // find the length of degrees
		strncpy(deg, latLon, len);              // copy the degree string to allow conversion to float
		dec = atof(deg) + atof(min) / 60;       // convert to float
		if (strcmp(direction, "S") == 0 || strcmp(direction, "W") == 0)
			dec *= -1;
	}
	return dec;
}





void AIwarning(void)
{
		fill_buffer(input_user_buffer);
	  neai_anomalydetection_detect(input_user_buffer, &similarity);
	  if(similarity>=90)
		{
			times =1;
		}
		else 
		{							 
			times =0;
		}
		
		if(times ==0)
		{	
			if(LightData_Hex<3){
			LED_R(1);
			vTaskDelay(40);
			LED_R(0);
			}else{
			BUZZER(1);
			vTaskDelay(100);
			BUZZER(0);}
		}else
		{
			LED_R (0);
			BUZZER(0);
		}			
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
