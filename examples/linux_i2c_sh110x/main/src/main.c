#include <stdio.h>
#include <linuxi2c.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <linux_i2c.h>

#define use_i2c_lib 0

#if use_i2c_lib == 0
LINUXI2CDevice i2cdev;
#else
void *i2cdev;
#endif

void i2c_del()
{
#if use_i2c_lib == 0
	int ret;
	if (i2cdev.bus != -1)
	{
		linuxi2c_close(i2cdev.bus);
		i2cdev.bus = -1;
	}
#else
    linux_i2c_destroy(i2cdev);
#endif
}

void i2c_init()
{
#if use_i2c_lib == 0
	int ret;
	const char *lines = "/dev/i2c-8";
	unsigned char addr = 0x3c;
	linuxi2c_init_device(&i2cdev);
	i2cdev.bus = linuxi2c_open(lines);
	if (i2cdev.bus == -1)
	{
		perror("Failed to open I2C bus");
		assert(i2cdev.bus != -1);
	}

	ret = linuxi2c_select(i2cdev.bus, addr, 0);
	if (ret != 0)
	{
		perror("Failed to select I2C device");
		assert(ret == 0);
	}

	i2cdev.addr = addr;
    i2cdev.delay = 0;
#else
    linux_i2c_init(8, 0x3c, &i2cdev);
#endif
}
#define I2C_CONTROL_BYTE_CMD_SINGLE    0x80
#define I2C_CONTROL_BYTE_CMD_STREAM    0x00
#define I2C_CONTROL_BYTE_DATA_SINGLE   0xC0
#define I2C_CONTROL_BYTE_DATA_STREAM   0x40
void oled_init()
{
#if use_i2c_lib == 0
	unsigned char buffer;
	buffer = 0xAE;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0x21;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0xDC;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0x00;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0xA0;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0xC8;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0xA8;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0x7F;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0xD3;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0x60;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0xD5;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0x50;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0xD9;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0xF1;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0xDB;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0x35;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0xAD;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0x81;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0x81;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0x80;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0xA4;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0xA6;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0xAF;linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
#else
    unsigned char buffer[2];
    buffer[0] = I2C_CONTROL_BYTE_CMD_SINGLE;
	buffer[1] = 0xAE;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x21;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xDC;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x00;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xA0;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xC8;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xA8;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x7F;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xD3;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x60;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xD5;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x50;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xD9;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xF1;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xDB;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x35;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xAD;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x81;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x81;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x80;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xA4;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xA6;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xAF;linux_i2c_write(i2cdev, NULL, 0, buffer, 2);
#endif
}

void oled_display_image(int page, int seg, uint8_t * images, int width)
{
	uint8_t columLow = seg & 0x0F;
	uint8_t columHigh = (seg >> 4) & 0x0F;

#if use_i2c_lib == 0
	unsigned char buffer;
	buffer = 0xB0 | page;		linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0x00 + columLow;	linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);
	buffer = 0x10 + columHigh;	linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, &buffer, 1);

    linuxi2c_ioctl_write(&i2cdev, I2C_CONTROL_BYTE_DATA_STREAM, images, width);
#else
    unsigned char buffer[2];
    buffer[0] = I2C_CONTROL_BYTE_CMD_SINGLE;
	buffer[1] = 0xB0 | page;		linux_i2c_write(i2cdev, buffer, 2, NULL, 0);
	buffer[1] = 0x00 + columLow;	linux_i2c_write(i2cdev, buffer, 2, NULL, 0);
	buffer[1] = 0x10 + columHigh;	linux_i2c_write(i2cdev, buffer, 2, NULL, 0);

    unsigned char tmp[1024];
    tmp[0] = I2C_CONTROL_BYTE_DATA_STREAM;
    memcpy(tmp + 1, images, width);
    linux_i2c_write(i2cdev, tmp, width + 1, NULL, 0);
#endif
}


int main()
{
    i2c_init();


    oled_init();

    uint8_t img[64];
    memset(img, 0xff, 64);
    for (int i = 0; i < 64; i++)
    {
        oled_display_image(0, i, img, 8);
        oled_display_image(7, i, img, 8);
    }
    

    // for (int p = 0; p < 16; p++)
    //     for (int s = 0; s < 64; s++)
    //     {
    //         oled_display_image(p, s, img, 1);
    //     }
    

    memset(img, 0, 64);
    for (int p = 4; p < 5; p++)
        for (int s = 27; s < 35; s++)
        {
            oled_display_image(p, s, img, 1);
        }
    
    for (int p = 11; p < 12; p++)
        for (int s = 27; s < 35; s++)
        {
            oled_display_image(p, s, img, 1);
        }

    // for (int p = 8; p < 9; p++)
    //     for (int s = 27; s < 35; s++)
    //     {
    //         oled_display_image(p, s, img, 1);
    //     }



    // for (int i = 0; i < 32; i++)
    // {
    //     oled_display_image(0, i, img, 8);
    // }
    // // for (uint8_t y = 0; y < 64; y++)
    // // {
    // //     oled_display_image(0, y, img, 64);
    // // }
    // oled_display_image(8, 0, img, 1);
    // memset(img, 0, 8);
    // // for (int i = 0; i < 8; i++)
    // // {
    // //     oled_display_image(0, i, img, 1);
    // // }
    
    
    // oled_display_image(1, 0, img, 1);

    // oled_display_image(3, 0, img, 1);

    // // oled_display_image(8, 0, img, 1);

    i2c_del();

    return 0;
}