#include "app.h"
#include "stdio.h"
#include "stm32f7xx.h"
#include "string.h"
#include "sfud.h"

static uint32_t app_exec_count = 0;

#define SFUD_DEMO_TEST_BUFFER_SIZE                     1024

static void sfud_demo(uint32_t addr, size_t size, uint8_t *data);

static uint8_t sfud_demo_test_buf[SFUD_DEMO_TEST_BUFFER_SIZE];

#define BUF_SIZE (256)
static uint8_t M25_WRITE_BUF[BUF_SIZE] = {0};
static uint8_t M25_READ_BUF[BUF_SIZE] = {0};

void app_loop(void)
{
    if (app_exec_count++ > 1000)
    {
    	app_exec_count = 0;

        LED1_TOGGLE;
        LED2_TOGGLE;

        for (uint32_t i=0; i<BUF_SIZE; i++)
        {
            M25_WRITE_BUF[i] = i;
        }
        W25QXX_Write(M25_WRITE_BUF, 0, BUF_SIZE);

        W25QXX_Read(M25_READ_BUF, 0, BUF_SIZE);
        for (uint32_t i=0; i<32; i++)
        {
            printf("0x%x\r\n", M25_READ_BUF[i]);
        }
        printf("\r\n");

        memset(M25_READ_BUF, 0, BUF_SIZE);

        printf("time: %s:%s, fighter app is running\r\n", __DATE__, __TIME__);
    }
}

/**
 * SFUD demo for the first flash device test.
 *
 * @param addr flash start address
 * @param size test flash size
 * @param size test flash data buffer
 */
static void sfud_demo(uint32_t addr, size_t size, uint8_t *data) {
    sfud_err result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + 0;
    size_t i;
    /* prepare write data */
    for (i = 0; i < size; i++) {
        data[i] = i;
    }
    /* erase test */
    result = sfud_erase(flash, addr, size);
    if (result == SFUD_SUCCESS) {
        printf("Erase the %s flash data finish. Start from 0x%08lX, size is %ld.\r\n", flash->name, addr,
                size);
    } else {
        printf("Erase the %s flash data failed.\r\n", flash->name);
        return;
    }
    /* write test */
    result = sfud_write(flash, addr, size, data);
    if (result == SFUD_SUCCESS) {
        printf("Write the %s flash data finish. Start from 0x%08X, size is %ld.\r\n", flash->name, addr,
                size);
    } else {
        printf("Write the %s flash data failed.\r\n", flash->name);
        return;
    }
    /* read test */
    result = sfud_read(flash, addr, size, data);
    if (result == SFUD_SUCCESS) {
        printf("Read the %s flash data success. Start from 0x%08X, size is %ld. The data is:\r\n", flash->name, addr,
                size);
        printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
        for (i = 0; i < size; i++) {
            if (i % 16 == 0) {
                printf("[%08X] ", addr + i);
            }
            printf("%02X ", data[i]);
            if (((i + 1) % 16 == 0) || i == size - 1) {
                printf("\r\n");
            }
        }
        printf("\r\n");
    } else {
        printf("Read the %s flash data failed.\r\n", flash->name);
    }
    /* data check */
    for (i = 0; i < size; i++) {
        if (data[i] != i % 256) {
            printf("Read and check write data has an error. Write the %s flash data failed.\r\n", flash->name);
			break;
        }
    }
    if (i == size) {
        printf("The %s flash test is success.\r\n", flash->name);
    }
}
