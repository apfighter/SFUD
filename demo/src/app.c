#include "app.h"
#include "stdio.h"
#include "stm32f7xx.h"

static uint32_t app_exec_count = 0;

void app_loop(void)
{
    if (app_exec_count++ > 500)
    {
    	app_exec_count = 0;

        LED1_TOGGLE;
        LED2_TOGGLE;

        printf("time: %s:%s, fighter app is running\r\n", __DATE__, __TIME__);
    }
}
