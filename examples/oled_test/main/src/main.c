#include <stdio.h>
// #include "oled.h"
#include "c_periphery/i2c.h"
#include <unistd.h>
#include <string.h>




i2c_t *i2cdev;

// void OLED_Write_Cmd(uint8_t cmd)
// {
//     i2c_ioctl_write(i2cdev, 0x00, &cmd, 1);
// }
// /**
//  * @brief	OLED写入数据
//  * @param cmd - 待写入数据
//  * @note	移植时，请使用自己的底层API实现 
// */
// void OLED_Write_Dat(uint8_t dat)
// {
// 	i2c_ioctl_write(i2cdev, 0x40, &dat, 1);
// }

// void OLED_Delay(int ms)
// {
// 	usleep(ms * 1000);
// }





unsigned char SSD1306_MINIMAL_framebuffer[1024];
#define SSD1306_MINIMAL_SLAVE_ADDR 0x3c

/* Transfers the entire framebuffer in 64 I2C data messages */
void SSD1306_MINIMAL_transferFramebuffer() {
  unsigned char *p = SSD1306_MINIMAL_framebuffer;
  for (int i = 0; i < 64; i++) {
        // I2C_WRAPPER_beginTransmission(SSD1306_MINIMAL_SLAVE_ADDR);
        // I2C_WRAPPER_write(0x40);
		i2c_ioctl_write(i2cdev, 0x40, p, 16);
        // for(int i = 0; i < 16; i++) {
        //   I2C_WRAPPER_write(*p);
        //   p++;
        // }
        // I2C_WRAPPER_endTransmission();
  }  
}

/* Horizontal addressing mode maps to linear framebuffer */
void SSD1306_MINIMAL_setPixel(unsigned int x, unsigned int y) {
  x &= 0x7f;
  y &= 0x3f;
  SSD1306_MINIMAL_framebuffer[((y & 0xf8) << 4) + x] |= 1 << (y & 7);
}

void SSD1306_MINIMAL_init() {
  unsigned char initialization[] = {
    /* Enable charge pump regulator (RESET = ) */
	0xAE,
	0x00,
	0x12,
	0x40,
	0xB0,
	0x81,
	0xFF,
	0xA1,
	0xA6,
	0xA8,
	0x2F,
	0xC8,
	0xD3,
	0x00,
	0xD5,
	0x80,
	0xD9,
	0x21,
	0xDA,
	0x12,
	0xDB,
	0x40,
	0x8D,
	0x14,
	0xA1,
	0xC8,
	0xAF
  };

//   I2C_WRAPPER_beginTransmission(SSD1306_MINIMAL_SLAVE_ADDR);
  for (int i = 0; i < sizeof(initialization); i++) {
    // I2C_WRAPPER_write(0x80);
    // I2C_WRAPPER_write(initialization[i]);
	i2c_ioctl_write(i2cdev, 0x80, &initialization[i], 1);
  }
//   I2C_WRAPPER_endTransmission();
}





// I2C_BUS=8
// SSD1306_ADDR=0x3C

int main(int argc, char *argv[])
{
    // printf("hello world!\n");
    i2cdev = i2c_new();
    i2c_open(i2cdev, "/dev/i2c-8");
    i2c_slave_address(i2cdev, 0x3C);


	SSD1306_MINIMAL_init();
	memset(SSD1306_MINIMAL_framebuffer, 0, 1024);




  const int height = 64;
  const int width = 128;
  const int max = 15;

  /* Let's draw the mandelbrot set on screen */
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      float c_re = (col - width / 2.0) * 4.0 / width;
      float c_im = (row - height / 2.0) * 4.0 / width;
      float x = 0, y = 0;
      int iteration = 0;
      while (x * x + y * y <= 4 && iteration < max) {
          float x_new = x * x - y * y + c_re;
          y = 2 * x * y + c_im;
          x = x_new;
          iteration++;
      }
      if (iteration == max) {
        /* This is where we actually set a pixel */
        SSD1306_MINIMAL_setPixel(-col - 30, row);
      }
    }
  }
  
  /* Transfer framebuffer to device */
  SSD1306_MINIMAL_transferFramebuffer();








    i2c_free(i2cdev);

    return 0;
}