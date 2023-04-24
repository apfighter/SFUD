#include "app.h"
#include "stm32f7xx.h"
#include "string.h"
#include "sfud.h"


static inline void spi_lock(const sfud_spi *spi)
{
    __disable_irq();
}

static inline void spi_unlock(const sfud_spi *spi)
{
    __enable_irq();
}

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
                               size_t read_size)
{
    sfud_err result = SFUD_SUCCESS;
    uint8_t send_data, read_data;

    if (write_size)
    {
        SFUD_ASSERT(write_buf);
    }
    if (read_size)
    {
        SFUD_ASSERT(read_buf);
    }

    SPI4_CS_LOW();

    /* 开始读写数据 */
    for (size_t i = 0; i < write_size + read_size; i++)
    {
        /* 先写缓冲区中的数据到 SPI 总线，数据写完后，再写 dummy(0xFF) 到 SPI 总线 */
        if (i < write_size)
        {
            send_data = *write_buf++;
        }
        else
        {
            send_data = SFUD_DUMMY_DATA;
        }

        HAL_SPI_TransmitReceive(&hspi4, &send_data, &read_data, 1, 0xFF);

        /* 写缓冲区中的数据发完后，再读取 SPI 总线中的数据到读缓冲区 */
        if (i >= write_size)
        {
            *read_buf++ = read_data;
        }
    }

    SPI4_CS_HIGH();

    return result;
}

#ifdef SFUD_USING_QSPI

/**
 * read flash data by QSPI
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format,
                          uint8_t *read_buf, size_t read_size)
{
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your qspi read flash data code
     */

    return result;
}

#endif /* SFUD_USING_QSPI */

static void retry_delay_100us()
{ // TODO
    uint32_t delay = 120;
    while (delay--);
}

#define SFUD_DEMO_TEST_BUFFER_SIZE 1024
static void sfud_demo(uint32_t addr, size_t size, uint8_t *data);
static uint8_t sfud_demo_test_buf[SFUD_DEMO_TEST_BUFFER_SIZE];

static uint32_t app_exec_count = 0;
static uint32_t sfud_inited = 0;

static sfud_flash flash;

void app_loop(void)
{
    if (sfud_inited == 0)
    {

        flash.spi.wr = spi_write_read;
        flash.spi.lock = spi_lock;
        flash.spi.unlock = spi_unlock;
#ifdef SFUD_USING_QSPI
        flash.spi.qspi_read = qspi_read;
#endif
        flash.spi.user_data = NULL;
        /* about 100 microsecond delay */
        flash.retry.delay = retry_delay_100us;
        /* adout 60 seconds timeout */
        flash.retry.times = 60 * 10000;

        sfud_device_register("MX25L128", &flash);

        sfud_inited = 1;

        /* SFUD initialize */
        if (sfud_init() == SFUD_SUCCESS)
        {
            sfud_demo(0, sizeof(sfud_demo_test_buf), sfud_demo_test_buf);
        }
    }
    if (app_exec_count++ > 1000)
    {
        app_exec_count = 0;

        LED1_TOGGLE;
        LED2_TOGGLE;

        printf("time: %s:%s, app is running\r\n", __DATE__, __TIME__);
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
    sfud_err result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + 0;
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
