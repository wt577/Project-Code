#ifndef _LED_H
#define _LED_H
#include "sys.h"

#define LED_R(n)			(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET))
#define LED_R_TogglePin		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_6)



#define PWRKEY(n)			(n?HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET))
#define RESET(n)			(n?HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET))

void LED_Init(void);
void POWER_RESET_Init(void);
#endif




