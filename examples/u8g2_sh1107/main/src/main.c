// #include <linux-i2c.h>
#include <u8g2.h>
#include <stdio.h>
#include <time.h>
#include <sample_log.h>


#define SSD1306_ADDR 0x3c

u8g2_t u8g2;

#define BUFSIZ_I2C 32

char filename[255];
uint8_t data[BUFSIZ_I2C]; // just to be sure
int idx = 0;
// almost certainly the wrong place for this state!
int file = 1;
uint8_t addr = 0x3c;
int adapter_nr = 8; /* probably dynamically determined */

uint8_t my_u8x8_byte_linux_i2c(u8x8_t *u8x8,
							   uint8_t msg,
							   uint8_t arg_int,
							   void *arg_ptr)
{
	switch (msg)
	{
	case U8X8_MSG_BYTE_SEND:
	{
		fprintf(stderr, "-- %d bytes:\n", arg_int);
		for (int i = 0; i < arg_int; i++)
		{
			printf("%x ", *((uint8_t *)(arg_ptr) + i));
		}
		printf("\n");
		// for (int i = 0; i < arg_int && idx < BUFSIZ_I2C; i++, idx++)
		// {
		// 	// data[idx] = *(uint8_t *)(arg_ptr+i);
		// 	data[idx] = *((uint8_t *)(arg_ptr) + i);
		// 	fprintf(stderr, "    %d  %d:  %0x\n", i, idx, data[idx]);
		// }
		// idx++;
	}
	break;
	case U8X8_MSG_BYTE_INIT:
	{ // ths open/setup? it seems to be a one-time setup
		snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
		ALOGI("%s", filename);
		// file = open(filename, O_RDWR);
		// if (file < 0) {
		// 	fprintf(stderr, "can't open i2c\n");
		// 	return(errno);
		// }
		// fprintf(stderr, "opened i2c file %d\n", file);
		// if (ioctl(file, I2C_SLAVE, addr) < 0) { // u8x8_GetI2CAddress(u8x8)
		// 	fprintf(stderr, "can't set addr %0x\n", addr);
		// 	return(errno);
		// }
		// fprintf(stderr, "set i2c addr %0x\n", addr);
	}
	break;
	case U8X8_MSG_BYTE_SET_DC:
		{ /* ignored for i2c */
			// fprintf(stderr, "++ set dc?\n");
			ALOGI("++ set dc?");
		}
		break;
	case U8X8_MSG_BYTE_START_TRANSFER:
		{
			ALOGI("++ start transfer, resetting buffers");
			// fprintf(stderr, "++ start transfer, resetting buffers\n");
			// memset(data, 0, BUFSIZ_I2C);
			// idx = 0;
		}
		break;
	case U8X8_MSG_BYTE_END_TRANSFER:
		{
			ALOGI("++ end transfer, sending cmd %0x %0x count %d", data[0], data[1], idx);
			// fprintf(stderr, "++ end transfer, sending cmd %0x %0x count %d\n", data[0], data[1], idx);
			// NB! note the extre _i2c_ in there! leave that out and you are screwed
			// if (i2c_smbus_write_i2c_block_data(file, data[0], idx - 1, &data[1]) < 0) {
			// 	fprintf(stderr, "can't write cmd %0x: %s\n",  data[0], strerror(errno));
			// 	return(errno);
			// }
		}
		break;
	default:
		fprintf(stderr, "unknown msg type %d\n", msg);
		return 0;
	}
	return 1;
}

uint8_t
my_u8x8_linux_i2c_delay(u8x8_t *u8x8,
						uint8_t msg,
						uint8_t arg_int,
						void *arg_ptr)
{
	struct timespec req;
	struct timespec rem;
	int ret;

	req.tv_sec = 0;

	switch (msg)
	{
	case U8X8_MSG_DELAY_NANO: // delay arg_int * 1 nano second
		req.tv_nsec = arg_int;
		break;
	case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
		req.tv_nsec = arg_int * 100;
		break;
	case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
		req.tv_nsec = arg_int * 10000;
		break;
	case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
		req.tv_nsec = arg_int * 1000000;
		break;
	default:
		return 0;
	}

	// while((ret = nanosleep(&req, &rem)) && errno == EINTR){
	// 	struct timespec tmp = req;
	// 	req = rem;
	// 	rem = tmp;
	// }
	// if (ret) {
	// 	perror("nanosleep");
	// 	fprintf(stderr, "can't sleep\n");
	// 	return(errno);
	// }

	return 1;
}

int main(void)
{
	//	u8g2_Setup_ssd1305_i2c_128x32_noname_1(&u8g2, U8G2_R0, u8x8_byte_linux_i2c, u8x8_linux_i2c_delay);
	//	u8g2_Setup_ssd1305_i2c_128x32_adafruit_1(&u8g2, U8G2_R0, u8x8_byte_linux_i2c, u8x8_linux_i2c_delay);
	// u8g2_Setup_ssd1306_i2c_128x32_univision_1(&u8g2, U8G2_R0, u8x8_byte_linux_i2c, u8x8_linux_i2c_delay);

	//	u8g2_Setup_ssd1305_i2c_128x32_noname_2(&u8g2, U8G2_R0, u8x8_byte_linux_i2c, u8x8_linux_i2c_delay);
	// 	u8g2_Setup_ssd1305_i2c_128x32_adafruit_2(&u8g2, U8G2_R0, u8x8_byte_linux_i2c, u8x8_linux_i2c_delay);
	//     u8g2_Setup_ssd1306_i2c_128x32_univision_2(&u8g2, U8G2_R0, u8x8_byte_linux_i2c, u8x8_linux_i2c_delay);

	// 	u8g2_Setup_ssd1305_i2c_128x32_noname_f(&u8g2, U8G2_R0, u8x8_byte_linux_i2c, u8x8_linux_i2c_delay);
	// 	u8g2_Setup_ssd1305_i2c_128x32_adafruit_f(&u8g2, U8G2_R0, u8x8_byte_linux_i2c, u8x8_linux_i2c_delay);
	//     u8g2_Setup_ssd1306_i2c_128x32_univision_f(&u8g2, U8G2_R0, u8x8_byte_linux_i2c, u8x8_linux_i2c_delay);

	// void u8g2_Setup_sh1107_i2c_64x128_1(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb, u8x8_msg_cb gpio_and_delay_cb);
	// void u8g2_Setup_sh1107_i2c_64x128_2(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb, u8x8_msg_cb gpio_and_delay_cb);
	// void u8g2_Setup_sh1107_i2c_64x128_f(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb, u8x8_msg_cb gpio_and_delay_cb);

	// u8g2_Setup_sh1107_i2c_64x128_1(&u8g2, U8G2_R0, my_u8x8_byte_linux_i2c, my_u8x8_linux_i2c_delay);
	// u8g2_Setup_sh1107_i2c_64x128_2(&u8g2, U8G2_R0, my_u8x8_byte_linux_i2c, my_u8x8_linux_i2c_delay);
	// u8g2_Setup_sh1107_i2c_64x128_f(&u8g2, U8G2_R0, my_u8x8_byte_linux_i2c, my_u8x8_linux_i2c_delay);


  uint8_t tile_buf_height;
  uint8_t *buf;
  u8g2_SetupDisplay(&u8g2, u8x8_d_sh1107_64x128, u8x8_cad_ssd13xx_fast_i2c, byte_cb, gpio_and_delay_cb);
  buf = u8g2_m_8_16_f(&tile_buf_height);
  u8g2_SetupBuffer(&u8g2, buf, tile_buf_height, u8g2_ll_hvline_vertical_top_lsb, rotation);






	u8g2_SetI2CAddress(&u8g2, SSD1306_ADDR);

	u8g2_InitDisplay(&u8g2);

	u8g2_SetPowerSave(&u8g2, 0);

	u8g2_ClearBuffer(&u8g2);

	u8g2_SetFont(&u8g2, u8g2_font_smart_patrol_nbp_tr);

	u8g2_SetFontRefHeightText(&u8g2);

	u8g2_SetFontPosTop(&u8g2);

	u8g2_DrawStr(&u8g2, 0, 0, "u8g2 Linux I2C");

	u8g2_SendBuffer(&u8g2);
}
