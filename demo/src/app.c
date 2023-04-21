#include "app.h"
#include "stdio.h"
#include "stm32f7xx.h"

static uint32_t app_exec_count = 0;

void app_loop(void)
{
    if (app_exec_count++ > 5000)
    {
        LED1_TOGGLE;
        LED2_TOGGLE;
    }
}
