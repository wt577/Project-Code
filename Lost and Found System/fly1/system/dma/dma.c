/**
  ******************************************************************************
  * File Name          : dma.c
  * Description        : This file provides code for the configuration
  *                      of all the requested memory to memory DMA transfers.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "dma.h"


/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure DMA                                                              */
/*----------------------------------------------------------------------------*/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** 
  * Enable DMA controller clock
  */
void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
	
	DMA1_TX_Handler.Instance = DMA1_Channel2;
	DMA1_TX_Handler.Init.Request = DMA_REQUEST_USART3_TX;  // 必须指定请求源
	DMA1_TX_Handler.Init.Direction = DMA_PERIPH_TO_MEMORY;
	DMA1_TX_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
	DMA1_TX_Handler.Init.MemInc = DMA_MINC_ENABLE;			// 内存地址递增
	DMA1_TX_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	DMA1_TX_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	DMA1_TX_Handler.Init.Mode = DMA_CIRCULAR;  // 普通模式
	DMA1_TX_Handler.Init.Priority = DMA_PRIORITY_HIGH;
	
	HAL_DMA_Init(&DMA1_TX_Handler);



	    // 绑定DMA与USART
	__HAL_LINKDMA(&UART3_Handler, hdmarx, DMA1_TX_Handler);
	
	
	HAL_UART_Receive_DMA(&UART3_Handler, (uint8_t*)dma_rx_buffer, USART3_REC_LEN);

  /* DMA interrupt init */
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
