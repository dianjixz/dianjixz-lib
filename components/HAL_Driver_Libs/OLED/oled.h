#ifndef __OLED_H
#define __OLED_H

// #include "i2c.h"


/* OLED控制用函数 */
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void);
void OLED_On(void);

/* OLED功能函数 */
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char size);
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size);
void OLED_ShowString(unsigned char x,unsigned char y, char *chr,unsigned char size);	 
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);

/* OLED初始化 */
void OLED_Init(void);

#endif  
