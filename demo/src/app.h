#ifndef __APP_H
#define __APP_H

#include "main.h"

#define LED1_ON     HAL_GPIO_WritePin(GPIOD, LED1_Pin, GPIO_PIN_RESET)
#define LED1_OFF    HAL_GPIO_WritePin(GPIOD, LED1_Pin, GPIO_PIN_SET)
#define LED1_TOGGLE HAL_GPIO_TogglePin(GPIOD, LED1_Pin)

#define LED2_ON     HAL_GPIO_WritePin(GPIOD, LED2_Pin, GPIO_PIN_RESET)
#define LED2_OFF    HAL_GPIO_WritePin(GPIOD, LED2_Pin, GPIO_PIN_SET)
#define LED2_TOGGLE HAL_GPIO_TogglePin(GPIOD, LED2_Pin)

void app_loop(void);

#endif //__APP_H
