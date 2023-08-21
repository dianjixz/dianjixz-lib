# CROSS_DIR ?= /opt/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin
# CROSS ?= arm-linux-gnueabihf-
# SSH_PASSWORLD:=

lvgl_e:
	@cp main/include/lv_conf.h ../../github_source/lvgl
	@cp main/include/lv_drv_conf.h ../../github_source/lvgl
	@make  target


include ../../tools/Makefile.mk
