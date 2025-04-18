/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <unistd.h>
#include <lv_drivers/display/fbdev.h>
#include <lv_drivers/indev/evdev.h>
#include "lvgl/lvgl.h"
#include "ui/ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void hal_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

 void _ui_init(void) {
  lv_disp_t *dispp  = lv_disp_get_default();
  lv_theme_t *theme = lv_theme_default_init(
      dispp, lv_palette_main(LV_PALETTE_BLUE),
      lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
  lv_disp_set_theme(dispp, theme);

  lv_obj_t *ui_Screen1 = lv_obj_create(NULL);
  lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);  /// Flags

  lv_obj_t *ui_Label17 = lv_label_create(ui_Screen1);
  lv_obj_set_width(ui_Label17, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_height(ui_Label17, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_align(ui_Label17, LV_ALIGN_CENTER);
  lv_label_set_text(ui_Label17, "hello world");

  lv_obj_t *ui____initial_actions0 = lv_obj_create(NULL);
  lv_disp_load_scr(ui_Screen1);
}
void axera_vivo_exit();
void all_exit()
{
  axera_vivo_exit();
}

#include <pthread.h>
pthread_mutex_t ui_thread_mutex;
int gLoopExit = 1;
int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/
  pthread_mutex_init(&ui_thread_mutex, NULL);
  // system("/soc/scripts/usb-adb.sh stop");
  // system("/soc/scripts/usb-acm.sh start");
  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  hal_init();

  ui_init();
  int count = 0;

  while(gLoopExit) {
      /* Periodically call the lv_task handler.
       * It could be done in a timer interrupt or an OS task too.*/
      pthread_mutex_lock(&ui_thread_mutex);
      
      lv_timer_handler();
      // printf("\r--%d", count++);fflush(stdout);
      pthread_mutex_unlock(&ui_thread_mutex);
      usleep(5 * 1000);
      
  }
  all_exit();
  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
lv_color_t *screen_buf = NULL;
void axeradev_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);
void axeradev_init();

/**
 * Initialize the Hardware Abstraction Layer (HAL) for LVGL
 */
static void hal_init(void)
{
  /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
  // sdl_init();
  // int fd = open("/sys/class/graphics/fbcon/cursor_blink", O_WRONLY);
  // char data = '0';
  // write(fd, &data, 1);
  // close(fd);
  system(
    "[ -d \"/sys/class/gpio/gpio511\" ] || { echo 511 > /sys/class/gpio/export ; echo out > "
    "/sys/class/gpio/gpio511/direction ; } ;"
    "echo 1  > /sys/class/gpio/gpio511/value ;");
  system("modprobe bmi270_i2c");
  axeradev_init();

  uint32_t width = 1280;
  uint32_t height = 720;
  /*Create a display buffer*/
  screen_buf = (lv_color_t *)malloc(width * height * sizeof(lv_color_t));
  static lv_disp_draw_buf_t disp_draw_buf;
  lv_disp_draw_buf_init(&disp_draw_buf, screen_buf, NULL, width * height);

  /*Create a display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv); /*Basic initialization*/
  disp_drv.draw_buf = &disp_draw_buf;
  disp_drv.flush_cb = axeradev_flush;
  disp_drv.hor_res = width;
  disp_drv.ver_res = height;
  lv_disp_drv_register(&disp_drv);


  evdev_init();
  // /* Add a mouse as input device */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv); /*Basic initialization*/
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = evdev_read;
  lv_indev_t *indev = lv_indev_drv_register(&indev_drv);
  LV_IMG_DECLARE(mouse_cursor_icon);                  /*Declare the image file.*/
  lv_obj_t *cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
  lv_img_set_src(cursor_obj, &mouse_cursor_icon);     /*Set the image source*/
  lv_indev_set_cursor(indev, cursor_obj);             /*Connect the image  object to the driver*/
  
}

#include "time.h"
uint32_t ___millis(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint32_t tick = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    return tick;
}
