#include "app.h"
#include "stm32f7xx.h"
#include "string.h"
#include "sfud.h"
#include "ff.h"

#define SFUD_DEMO_TEST_BUFFER_SIZE 32
static void sfud_demo(uint32_t addr, size_t size, uint8_t *data);
static uint8_t sfud_demo_test_buf[SFUD_DEMO_TEST_BUFFER_SIZE];

static uint32_t app_exec_count = 0;
static uint32_t sfud_inited    = 0;

extern void lfs_test(void);

// static uint8_t test_data = 0;
// static uint8_t rx_test_buf[256];
// static uint8_t tx_test_buf[256];

FATFS fs_obj;             /* Filesystem object */

void fs_test(void)
{
    FIL fil;              /* File object */
    FRESULT res;          /* API result code */
    UINT bw;              /* Bytes written */
    BYTE mm[512];
    UINT i;

    printf("fatfs test start:\r\n");
    /* 挂载文件系统 */
    res = f_mount(&fs_obj, "0:", 1);
    if (res)
    {
        printf("fatfs mount fail.\r\n");
        /* 格式化文件系统 */
        res = f_mkfs("0:", 0, 0); //"0:"是卷标，来自于 #define SPI_FLASH 0
        if (res)
        {
            printf("fatfs format fail.\r\n");
            return;
        }
        else
        {
            printf("fatfs format success.\r\n");
            f_mount(NULL, "0:", 1);
            res = f_mount(&fs_obj, "0:", 1);
            if (res)
            {
                printf("fatfs mount fail.\r\n");
            }
            else
            {
                printf("fatfs mount success.\r\n");
            }
        }
    }
    else
    {
        printf("fatfs mount success.\r\n");
    }
    /* Create a file as new */
    res = f_open(&fil, "0:/1.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
    if (res)
    {
        printf("fatfs file open fail. res = %d\r\n", res);
        return;
    }
    else
    {
        printf("fatfs file open success.\r\n");
    }
    /* Write a message */
    for (uint32_t i=0; i<128; i++)
    {
        mm[i] = i;
    }
    res = f_write(&fil, mm, 128, &bw);
    // printf("res write:%d\r\n",res);
    if (bw == 128)
    {
        printf("fatfs file write success!\r\n");
    }
    else
    {
        printf("fatfs file write fail!\r\n");
        return;
    }
    res = f_size(&fil);
    printf("file size: %d Bytes.\r\n", res);
    memset(mm, 0x0, 128);
    f_lseek(&fil, 0);
    res = f_read(&fil, mm, 128, &i);
    if (res == FR_OK)
    {
        printf("fatfs read file success!\r\n");
        printf("read szie: %d Bytes.\r\n", i);
    }
    else
    {
        printf("fatfs read file fail!\r\n");
    }
    for (uint32_t i=0; i<32; i++)
    {
        printf("0x%x ", mm[i]);
    }
    printf("\r\n");
    /* Close the file */
    f_close(&fil);
    /*卸载文件系统*/
    printf("fatfs test ok.\r\n");
}

void app_loop(void)
{
    if (sfud_inited == 0)
    {
        sfud_inited = 1;

        if (sfud_init() == SFUD_SUCCESS) /* SFUD initialize */
        {
            uint32_t i=100000;
            while(i--);
            // sfud_demo(0, sizeof(sfud_demo_test_buf), sfud_demo_test_buf);
            fs_test();
        }
    }

    if (app_exec_count++ > 2000)
    {
        // test_data++;
        app_exec_count = 0;

        // lfs_test();

        LED1_RUN_TOGGLE;
        LED2_RUN_TOGGLE;

        // W25QXX_Read((uint8_t *)rx_test_buf, 0, sizeof(rx_test_buf));
        // for (uint8_t i=0; i<16; i++)
        // {
        //     printf("%d ", rx_test_buf[i]);
        // }
        // printf("\r\n");

        // memset(tx_test_buf, test_data, sizeof(tx_test_buf));
        // W25QXX_Write((uint8_t *)tx_test_buf, 0, sizeof(tx_test_buf));

        sfud_log_info("time: %s:%s, app is running", __DATE__, __TIME__);

        // printf("\r\n");
    }
}

/**
 * SFUD demo for the first flash device test.
 *
 * @param addr flash start address
 * @param size test flash size
 * @param size test flash data buffer
 */
static void sfud_demo(uint32_t addr, size_t size, uint8_t *data)
{
    sfud_err result            = SFUD_SUCCESS;
    const    sfud_flash *flash = sfud_get_device_table() + 0;
    size_t i;

    /* prepare write data */
    for (i = 0; i < size; i++)
    {
        data[i] = i;
    }

    /* erase test */
    result = sfud_erase(flash, addr, size);
    if (result == SFUD_SUCCESS)
    {
        printf("Erase the %s flash data finish. Start from 0x%08lX, size is %d.\r\n", flash->name, addr, size);
    }
    else
    {
        printf("Erase the %s flash data failed.\r\n", flash->name);
        return;
    }

    /* write test */
    result = sfud_write(flash, addr, size, data);
    if (result == SFUD_SUCCESS)
    {
        printf("Write the %s flash data finish. Start from 0x%08lX, size is %d.\r\n", flash->name, addr, size);
    }
    else
    {
        printf("Write the %s flash data failed.\r\n", flash->name);
        return;
    }

    /* read test */
    result = sfud_read(flash, addr, size, data);
    if (result == SFUD_SUCCESS)
    {
        printf("Read the %s flash data success. Start from 0x%08lX, size is %d. The data is:\r\n", flash->name, addr, size);
        printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
        for (i = 0; i < size; i++)
        {
            if (i % 16 == 0)
            {
                printf("[%08lX] ", addr + i);
            }
            printf("%02X ", data[i]);
            if (((i + 1) % 16 == 0) || i == size - 1)
            {
                printf("\r\n");
            }
        }
        printf("\r\n");
    }
    else
    {
        printf("Read the %s flash data failed.\r\n", flash->name);
    }

    /* data check */
    for (i = 0; i < size; i++)
    {
        if (data[i] != i % 256)
        {
            printf("Read and check write data has an error. Write the %s flash data failed.\r\n", flash->name);
            break;
        }
    }
    if (i == size)
    {
        printf("The %s flash test is success.\r\n", flash->name);
    }
}
