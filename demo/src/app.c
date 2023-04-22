#include "app.h"
#include "stdio.h"
#include "stm32f7xx.h"
#include "string.h"

static uint32_t app_exec_count = 0;

// #define BUF_SIZE (256)
// static uint8_t M25_WRITE_BUF[BUF_SIZE] = {0};
// static uint8_t M25_READ_BUF[BUF_SIZE] = {0};

void app_loop(void)
{
    if (app_exec_count++ > 1000)
    {
    	app_exec_count = 0;

        LED1_TOGGLE;
        LED2_TOGGLE;

        // for (uint32_t i=0; i<BUF_SIZE; i++)
        // {
        //     M25_WRITE_BUF[i] = i;
        // }
        // W25QXX_Write(M25_WRITE_BUF, 0, BUF_SIZE);

        // W25QXX_Read(M25_READ_BUF, 0, BUF_SIZE);
        // for (uint32_t i=0; i<32; i++)
        // {
        //     printf("0x%x\r\n", M25_READ_BUF[i]);
        // }
        // printf("\r\n");

        // memset(M25_READ_BUF, 0, BUF_SIZE);

        printf("time: %s:%s, fighter app is running\r\n", __DATE__, __TIME__);
    }
}
