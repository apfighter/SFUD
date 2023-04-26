#ifndef _LFS_PORT_H_
#define _LFS_PORT_H_

#include "lfs.h"
#include "w25qxx.h"

/**
 * lfs与底层flash读数据接口
 * @param  c
 * @param  block  块编号
 * @param  off    块内偏移地址
 * @param  buffer 用于存储读取到的数据
 * @param  size   要读取的字节数
 * @return
 */
static int lfs_deskio_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    W25QXX_Read((uint8_t *)buffer, c->block_size * block + off, size);
    return LFS_ERR_OK;
}

/**
 * lfs与底层flash写数据接口
 * @param  c
 * @param  block  块编号
 * @param  off    块内偏移地址
 * @param  buffer 待写入的数据
 * @param  size   待写入数据的大小
 * @return
 */
static int lfs_deskio_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    W25QXX_Write_NoCheck((uint8_t *)buffer, c->block_size * block + off, size);
    return LFS_ERR_OK;
}

/**
 * lfs与底层flash擦除接口
 * @param  c
 * @param  block 块编号
 * @return
 */
static int lfs_deskio_erase(const struct lfs_config *c, lfs_block_t block)
{
    W25QXX_Erase_Sector(block);
    return LFS_ERR_OK;
}

static int lfs_deskio_sync(const struct lfs_config *c)
{
    return LFS_ERR_OK;
}

// lfs句柄
lfs_t lfs_w25qxx;

lfs_file_t lfs_file_w25qxx;

const struct lfs_config cfg =
{
    // block device operations
    .read  = lfs_deskio_read,
    .prog  = lfs_deskio_prog,
    .erase = lfs_deskio_erase,
    .sync  = lfs_deskio_sync,

    // block device configuration
    .read_size      = 16,
    .prog_size      = 16,
    .block_size     = 4096,
    .block_count    = 128,
    .cache_size     = 16,
    .lookahead_size = 16,
    .block_cycles   = 500,
};

// entry point
void lfs_test(void)
{
    // mount the filesystem
    int err = lfs_mount(&lfs_w25qxx, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err)
    {
        lfs_format(&lfs_w25qxx, &cfg);
        lfs_mount(&lfs_w25qxx, &cfg);
    }

    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(&lfs_w25qxx, &lfs_file_w25qxx, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs_w25qxx, &lfs_file_w25qxx);
    lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs_w25qxx, &lfs_file_w25qxx);

    // release any resources we were using
    lfs_unmount(&lfs_w25qxx);

    // print the boot count
    printf("boot_count: %d\n", boot_count);
}

#endif //_LFS_PORT_H_
