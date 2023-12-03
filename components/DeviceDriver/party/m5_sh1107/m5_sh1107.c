
#include "m5_sh1107.h"
#include "linux_i2c/linux_i2c.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
void *i2cdev;

int m5_sh1107_dev_deinit()
{
    linux_i2c_destroy(i2cdev);
    return 0;
}

int m5_sh1107_dev_init(int num)
{
    int ret = linux_i2c_init(num, 0x3c, &i2cdev);
    if(ret != 0 )
    {
        printf("m5_sh1107_dev_init i2c init faile!\n");
        return -1;
    }
    unsigned char buffer[2];
    buffer[0] = I2C_CONTROL_BYTE_CMD_SINGLE;
	buffer[1] = 0xAE;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x21;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xDC;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x00;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xA0;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xC8;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xA8;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x7F;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xD3;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x60;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xD5;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x50;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xD9;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xF1;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xDB;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x35;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xAD;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x81;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x81;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x80;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xA4;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xA6;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xAF;   linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
    return 0;
}

int m5_sh1107_dev_set_img(int page, int seg, void * images, int width)
{
	uint8_t columLow = seg & 0x0F;
	uint8_t columHigh = (seg >> 4) & 0x0F;

    unsigned char buffer[2];
    buffer[0] = I2C_CONTROL_BYTE_CMD_SINGLE;
	buffer[1] = 0xB0 | page;		linux_i2c_write(i2cdev, buffer, 2, NULL, 0);
	buffer[1] = 0x00 + columLow;	linux_i2c_write(i2cdev, buffer, 2, NULL, 0);
	buffer[1] = 0x10 + columHigh;	linux_i2c_write(i2cdev, buffer, 2, NULL, 0);

    unsigned char tmp[1024];
    tmp[0] = I2C_CONTROL_BYTE_DATA_STREAM;
    memcpy(tmp + 1, images, width);
    linux_i2c_write(i2cdev, tmp, width + 1, NULL, 0);
    return 0;
}

