/* Flash multiple partitions example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "pigpio.h"
#include <sys/param.h>
#include "raspberry_port.h"
#include "esp_loader.h"
#include "example_common.h"

#define TARGET_RST_Pin 21
#define TARGET_IO0_Pin 22

#define DEFAULT_BAUD_RATE 115200
#define HIGHER_BAUD_RATE  460800
#define SERIAL_DEVICE     "/dev/ttyS1"

// pin 21 (UART3_TXD) ESP_EN 0x02304084
// pin 22 (UART3_RXD) ESP_BOOT 0x02304090
int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("please run :linux_flasher esp32c6 bin\n");
    }
    example_binaries_t bin;

    const loader_raspberry_config_t config = {
        .device            = SERIAL_DEVICE,
        .baudrate          = DEFAULT_BAUD_RATE,
        .reset_trigger_pin = TARGET_RST_Pin,
        .gpio0_trigger_pin = TARGET_IO0_Pin,
    };

    loader_port_raspberry_init(&config);

    if (connect_to_target(HIGHER_BAUD_RATE) == ESP_LOADER_SUCCESS) {
        get_example_binaries(esp_loader_get_target(), &bin);
        // bin.boot.data = ESP32_C6_bootloader_bin;
        // bin.boot.size = ESP32_C6_bootloader_bin_size;
        // bin.boot.addr = 0x0;
        // bin.part.data = ESP32_C6_partition_table_bin;
        // bin.part.size = ESP32_C6_partition_table_bin_size;
        // bin.part.addr = 0x8000;
        // bin.app.data  = ESP32_C6_hello_world_bin;
        // bin.app.size  = ESP32_C6_hello_world_bin_size;
        // bin.app.addr  = 0x10000;
        FILE *file;
        char *bin_data;
        long file_size;
        {
            // 打开文件，以二进制读取模式打开
            file = fopen(argv[1], "rb");
            if (file == NULL) {
                perror("file not exsit\n");
                return 1;
            }

            // 获取文件大小
            fseek(file, 0, SEEK_END);
            file_size = ftell(file);
            rewind(file);

            // 分配内存以存储文件内容
            bin_data = (char *)malloc(sizeof(char) * file_size);
            if (bin_data == NULL) {
                perror("mem error");
                fclose(file);
                return 1;
            }

            // 读取文件内容到缓冲区
            if (fread(bin_data, 1, file_size, file) != file_size) {
                perror("file read error\n");
                free(bin_data);
                fclose(file);
                return 1;
            }

            fclose(file);
        }

        printf("Loading bin...\n");
        flash_binary(bin_data, file_size, 0);
        // printf("Loading partition table...\n");
        // flash_binary(bin.part.data, bin.part.size, bin.part.addr);
        // printf("Loading app...\n");
        // flash_binary(bin.app.data,  bin.app.size,  bin.app.addr);
        printf("Done!\n");
        esp_loader_reset_target();
        loader_port_deinit();
        free(bin_data);
        // if (gpioInitialise() < 0) {
        //     fprintf(stderr, "pigpio initialization failed\n");
        //     return 1;
        // }

        // int serial = serOpen(SERIAL_DEVICE, DEFAULT_BAUD_RATE, 0);
        // if (serial < 0) {
        //     printf("Serial port could not be opened!\n");
        // }

        // printf("********************************************\n");
        // printf("*** Logs below are print from slave .... ***\n");
        // printf("********************************************\n");

        // // Delay for skipping the boot message of the targets
        // gpioDelay(500000);
        // while (1) {
        //     int byte = serReadByte(serial);
        //     if (byte != PI_SER_READ_NO_DATA) {
        //         printf("%c", byte);
        //     }
        // }
    }
}
