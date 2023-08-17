#include <string.h>

#include <linuxi2c.h>
#include <assert.h>
#include "sh1107.h"

#define TAG "SH1107"

#define I2C_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 400000 /*!< I2C master clock frequency. no higher than 1MHz for now */

LINUXI2CDevice i2cdev;


void i2c_del()
{
	int ret;
	if (i2cdev.bus != -1)
	{
		linuxi2c_close(i2cdev.bus);
		i2cdev.bus = -1;
	}
}

void i2c_init()
{
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
}


uint8_t tmpbuff[1024 * 5];

void i2c_master_init(SH1107_t * dev, int16_t sda, int16_t scl, int16_t reset)
{
	// printf("I2CAddress:%x, i2c-id:%d\n", I2CAddress, dev->_dc);
	// dev->_address = I2CAddress;
    // int ret = i2c_init(dev->_dc, I2CAddress, &i2c_p);
    // if(ret != 0)
    // {
    //     printf("i2c init error!\n");
    // }
	i2c_init();
}

void _i2c_init(SH1107_t * dev, int width, int height)
{
	printf("I2CAddress:%x, i2c-id:%d\n", I2CAddress, dev->_dc);
	
	// int ret = i2c_init(dev->_dc, I2CAddress, &i2c_p);
	// if(ret != 0)
    // {
    //     printf("i2c init error!\n");
    // }
	dev->_width = width;
	dev->_height = height;
	dev->_pages = height / 8;
	dev->_direction = DIRECTION0;
	// i2c_master_init(dev, 0, 0, 0);
	unsigned char buffer;
	buffer = 0xAE;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0x21;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0xDC;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0x00;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0xA0;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0xC8;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0xA8;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0x7F;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0xD3;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0x60;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0xD5;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0x50;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0xD9;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0xF1;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0xDB;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0x35;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0xAD;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0x81;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0x81;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0x80;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0xA4;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0xA6;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0xAF;linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);


	// unsigned char data[2];

	// data[0] = I2C_CONTROL_BYTE_CMD_SINGLE;
    // data[1] = 0xAE;i2c_write(i2c_p, data, 2, NULL, 0);
	// data[1] = 0x21;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0xDC;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0x00;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0xA0;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0xC8;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0xA8;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0x7F;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0xD3;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0x60;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0xD5;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0x50;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0xD9;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0xF1;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0xDB;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0x35;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0xAD;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0x81;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0x81;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0x80;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0xA4;i2c_write(i2c_p, data, 2, NULL, 0);
    // data[1] = 0xA6;i2c_write(i2c_p, data, 2, NULL, 0);
	// data[1] = 0xAF;i2c_write(i2c_p, data, 2, NULL, 0);
    // // data = 0xB0;i2c_write(i2c_p, &reg, 1, &data, 1);
    // // data = 0x00;i2c_write(i2c_p, &reg, 1, &data, 1);
    // // data = 0x10;i2c_write(i2c_p, &reg, 1, &data, 1);


	// ret = i2c_write(i2c_p, data, sizeof(data), NULL, 0);
    // unsigned char data[] = {
	// 						I2C_CONTROL_BYTE_CMD_STREAM,
    //                         0xAE,
    //                         0xDC,
    //                         0x00,
    //                         0x81,
    //                         0x2F,
    //                         0x20,
    //                         0xA0,
    //                         0xC0,
    //                         0xA8,
    //                         0x7F,
    //                         0xD3,
    //                         0x60,
    //                         0xD5,
    //                         0x51,
    //                         0xD9,
    //                         0x22,
    //                         0xDB,
    //                         0x35,
    //                         0xB0,
    //                         0xDA,
    //                         0x12,
    //                         0xA4,
    //                         0xA6,
    //                         // 0xA7,
    //                         0xAF
    //                         };
    // ret = i2c_write(i2c_p, data, sizeof(data), NULL, 0);
	// if (ret == 0) {
	// 	// printf("OLED configured successfully\n");
	// } else {
	// 	printf("OLED configuration failed. code: 0x%X\n", ret);
	// 	i2c_destroy(i2c_p);
	// 	exit(-1);
	// }
	// i2c_destroy(i2c_p);
	// i2c_master_write_byte(cmd, (dev->_address << 1) | I2C_MASTER_WRITE, true);
	// i2c_master_write_byte(cmd, I2C_CONTROL_BYTE_CMD_STREAM, true);
	// i2c_master_write_byte(cmd, 0xAE, true);		// Turn display off
	// i2c_master_write_byte(cmd, 0xDC, true);		// Set display start line
	// i2c_master_write_byte(cmd, 0x00, true);		// ...value
	// i2c_master_write_byte(cmd, 0x81, true);		// Set display contrast
	// i2c_master_write_byte(cmd, 0x2F, true);		// ...value
	// i2c_master_write_byte(cmd, 0x20, true);		// Set memory mode
	// i2c_master_write_byte(cmd, 0xA0, true);		// Non-rotated display
	// i2c_master_write_byte(cmd, 0xC0, true);		// Non-flipped vertical
	// i2c_master_write_byte(cmd, 0xA8, true);		// Set multiplex ratio
	// i2c_master_write_byte(cmd, 0x7F, true);		// ...value
	// i2c_master_write_byte(cmd, 0xD3, true);		// Set display offset to zero
	// i2c_master_write_byte(cmd, 0x60, true);		// ...value
	// i2c_master_write_byte(cmd, 0xD5, true);		// Set display clock divider
	// i2c_master_write_byte(cmd, 0x51, true);		// ...value
	// i2c_master_write_byte(cmd, 0xD9, true);		// Set pre-charge
	// i2c_master_write_byte(cmd, 0x22, true);		// ...value
	// i2c_master_write_byte(cmd, 0xDB, true);		// Set com detect
	// i2c_master_write_byte(cmd, 0x35, true);		// ...value
	// i2c_master_write_byte(cmd, 0xB0, true);		// Set page address
	// i2c_master_write_byte(cmd, 0xDA, true);		// Set com pins
	// i2c_master_write_byte(cmd, 0x12, true);		// ...value
	// i2c_master_write_byte(cmd, 0xA4, true);		// output ram to display
	// i2c_master_write_byte(cmd, 0xA6, true);		// Non-inverted display
	// //i2c_master_write_byte(cmd, 0xA7, true);	// Inverted display
	// i2c_master_write_byte(cmd, 0xAF, true);		// Turn display on

	// i2c_master_stop(cmd);

	// esp_err_t espRc = i2c_master_cmd_begin(I2C_NUM, cmd, 10/portTICK_PERIOD_MS);
	// if (espRc == ESP_OK) {
	// 	ESP_LOGI(TAG, "OLED configured successfully");
	// } else {
	// 	ESP_LOGE(TAG, "OLED configuration failed. code: 0x%.2X", espRc);
	// }
	// i2c_cmd_link_delete(cmd);
}


void i2c_display_image(SH1107_t * dev, int page, int seg, uint8_t * images, int width)
{
	// i2c_cmd_handle_t cmd;

	if (page >= dev->_pages) return;
	if (seg >= dev->_width) return;
    // int ret = i2c_init(dev->_dc, I2CAddress, &i2c_p);
	uint8_t columLow = seg & 0x0F;
	uint8_t columHigh = (seg >> 4) & 0x0F;

	unsigned char buffer;
	buffer = 0xB0 | page;		linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0x00 + columLow;	linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);
	buffer = 0x10 + columHigh;	linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_SINGLE, buffer, 1);



	// unsigned char data[2];

	// data[0] = I2C_CONTROL_BYTE_CMD_SINGLE;
    // data[1] = 0xB0 | page;i2c_write(i2c_p, data, 2, NULL, 0);
	// data[1] = 0x00 + columLow;i2c_write(i2c_p, data, 2, NULL, 0);
	// data[1] = 0x10 + columHigh;i2c_write(i2c_p, data, 2, NULL, 0);

    // unsigned char data[] = {I2C_CONTROL_BYTE_CMD_STREAM,
    //                         (0x10 + columHigh),
    //                         (0x00 + columLow),
    //                         0xB0 | page
    //                         };
    // ret = i2c_write(i2c_p, data, sizeof(data), NULL, 0);
	// if (ret == 0) {
	// 	// printf("OLED i2c_display_image successfully\n");
	// } else {
	// 	printf("OLED i2c_display_image failed. code: 0x%X\n", ret);
	// }
	// i2c_destroy(i2c_p);

	// ret = i2c_init(dev->_dc, I2CAddress, &i2c_p);


	linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_DATA_STREAM, images, width);

	// tmpbuff[0] = I2C_CONTROL_BYTE_DATA_STREAM;
	// memcpy(tmpbuff + 1, images, width);
	// i2c_write(i2c_p, tmpbuff, width + 1, NULL, 0);



    // unsigned char reg1[] = {I2C_CONTROL_BYTE_DATA_STREAM};

    // ret = i2c_write(i2c_p, tmpbuff, width + 1, NULL, 0);
	// if (ret == 0) {
	// 	// printf("OLED i2c_display_image1 successfully\n");
	// } else {
	// 	printf("images:%p, width:%d\n", images, width);
	// 	printf("OLED i2c_display_image1 failed. code: 0x%X\n", ret);
	// }
	// i2c_destroy(i2c_p);
	// cmd = i2c_cmd_link_create();
	// i2c_master_start(cmd);
	// i2c_master_write_byte(cmd, (dev->_address << 1) | I2C_MASTER_WRITE, true);

	// i2c_master_write_byte(cmd, I2C_CONTROL_BYTE_CMD_STREAM, true);
	// // Set Higher Column Start Address for Page Addressing Mode
	// i2c_master_write_byte(cmd, (0x10 + columHigh), true);
	// // Set Lower Column Start Address for Page Addressing Mode
	// i2c_master_write_byte(cmd, (0x00 + columLow), true);
	// // Set Page Start Address for Page Addressing Mode
	// i2c_master_write_byte(cmd, 0xB0 | page, true);

	// i2c_master_stop(cmd);
	// i2c_master_cmd_begin(I2C_NUM, cmd, 10/portTICK_PERIOD_MS);
	// i2c_cmd_link_delete(cmd);

	// cmd = i2c_cmd_link_create();
	// i2c_master_start(cmd);
	// i2c_master_write_byte(cmd, (dev->_address << 1) | I2C_MASTER_WRITE, true);

	// i2c_master_write_byte(cmd, I2C_CONTROL_BYTE_DATA_STREAM, true);
	// i2c_master_write(cmd, images, width, true);

	// i2c_master_stop(cmd);
	// i2c_master_cmd_begin(I2C_NUM, cmd, 10/portTICK_PERIOD_MS);
	// i2c_cmd_link_delete(cmd);
}

void i2c_contrast(SH1107_t * dev, int contrast)
{
	// // i2c_cmd_handle_t cmd;
    // int ret = i2c_init(dev->_dc, I2CAddress, &i2c_p);
	int _contrast = contrast;
	if (contrast < 0x0) _contrast = 0;
	if (contrast > 0xFF) _contrast = 0xFF;
	// unsigned char data[2];

	unsigned char buffer;
	buffer = 0x81;		linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_STREAM, buffer, 1);
	buffer = _contrast;	linuxi2c_write(&i2cdev, I2C_CONTROL_BYTE_CMD_STREAM, buffer, 1);

    // unsigned char data[] = {I2C_CONTROL_BYTE_CMD_STREAM,
    //                         0x81,
    //                         _contrast
    //                         };
    // ret = i2c_write(i2c_p, data, sizeof(data), NULL, 0);
	// if (ret == 0) {
	// 	// printf("OLED i2c_contrast successfully\n");
	// } else {
	// 	printf("OLED i2c_contrast failed. code: 0x%X\n", ret);
	// }
	// i2c_destroy(i2c_p);
	// cmd = i2c_cmd_link_create();
	// i2c_master_start(cmd);
	// i2c_master_write_byte(cmd, (dev->_address << 1) | I2C_MASTER_WRITE, true);
	// i2c_master_write_byte(cmd, I2C_CONTROL_BYTE_CMD_STREAM, true);
	// i2c_master_write_byte(cmd, 0x81, true);		// Set Contrast Control Register
	// i2c_master_write_byte(cmd, _contrast, true);
	// i2c_master_stop(cmd);
	// i2c_master_cmd_begin(I2C_NUM, cmd, 10/portTICK_PERIOD_MS);
	// i2c_cmd_link_delete(cmd);
}