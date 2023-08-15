// #include <linux-i2c.h>

#include <stdio.h>
#include <time.h>
#include <M5UnitOLED.h>
// M5UnitOLED display; // default setting


// M5Canvas canvas(&display);
// static constexpr char text[] = "Hello world !";
// static constexpr size_t textlen = sizeof(text) / sizeof(text[0]);
// int textpos = 0;
// int scrollstep = 2;


int main(void)
{
	
	M5UnitOLED display(M5UNITOLED_SDA, M5UNITOLED_SCL, M5UNITOLED_FREQ, 8, M5UNITOLED_ADDR);
	
	display.begin();
	display.clear();

	// display.;


//   display.begin();
//   display.setColorDepth(1);

//   if (display.isEPD())
//   {
//     scrollstep = 1;
//     display.setEpdMode(epd_mode_t::epd_fastest);
//     display.invertDisplay(true);
//     display.clear(TFT_BLACK);
//   }
//   if (display.width() < display.height())
//   {
//     display.setRotation(display.getRotation() ^ 1);
//   }

//   canvas.setColorDepth(1); // mono color
//   canvas.setFont(&fonts::lgfxJapanMinchoP_32);
//   canvas.setTextWrap(false);
//   canvas.setTextSize(2);
//   canvas.createSprite(display.width() + 64, 72);

// while (1)
// {
//   int32_t cursor_x = canvas.getCursorX() - scrollstep;
//   if (cursor_x <= 0)
//   {
//     textpos = 0;
//     cursor_x = display.width();
//   }

//   canvas.setCursor(cursor_x, 0);
//   canvas.scroll(-scrollstep, 0);
//   while (textpos < textlen && cursor_x <= display.width())
//   {
//     canvas.print(text[textpos++]);
//     cursor_x = canvas.getCursorX();
//   }
//   display.waitDisplay();
//   int y = (display.height() - canvas.height()) >> 1;
// /*
//   display.copyRect(0, y, display.width(), canvas.height(), scrollstep, y);
//   display.setClipRect(display.width()-scrollstep, y, scrollstep, canvas.height());
//   canvas.pushSprite(&display, 0, (display.height() - canvas.height()) >> 1);
//   display.clearClipRect();
// /*/
//   canvas.pushSprite(&display, 0, y);
// }





	return 0;
}
