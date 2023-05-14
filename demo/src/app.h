#ifndef __APP_H
#define __APP_H

#include "main.h"
#include "w25qxx.h"
#include "stdio.h"
#include "stm32f7xx_hal_def.h"

#define LED1_RUN_ON      HAL_GPIO_WritePin(GPIOD, LED1_Pin, GPIO_PIN_RESET)
#define LED1_RUN_OFF     HAL_GPIO_WritePin(GPIOD, LED1_Pin, GPIO_PIN_SET)
#define LED1_RUN_TOGGLE  HAL_GPIO_TogglePin(GPIOD, LED1_Pin)

#define LED2_RUN_ON      HAL_GPIO_WritePin(GPIOD, LED2_Pin, GPIO_PIN_RESET)
#define LED2_RUN_OFF     HAL_GPIO_WritePin(GPIOD, LED2_Pin, GPIO_PIN_SET)
#define LED2_RUN_TOGGLE  HAL_GPIO_TogglePin(GPIOD, LED2_Pin)

#define MX25_CS_HIGH()   HAL_GPIO_WritePin(M25X_CS_GPIO_Port, M25X_CS_Pin, GPIO_PIN_SET)
#define MX25_CS_LOW()    HAL_GPIO_WritePin(M25X_CS_GPIO_Port, M25X_CS_Pin, GPIO_PIN_RESET)

void app_loop(void);

#endif //__APP_H
