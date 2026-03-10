#ifndef __BUZZER_H
#define __BUZZER_H


#define BUZZER(n)	(n?HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET))
#define BUZZER_TogglePin	HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_0)


void BUZZER_Init(void);




#endif


