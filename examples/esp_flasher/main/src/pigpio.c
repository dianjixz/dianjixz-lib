#include "pigpio.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
// pin 21 (UART3_TXD) ESP_EN 0x02304084
// pin 22 (UART3_RXD) ESP_BOOT 0x02304090
// void gpioSetMode(int io, int mode) {
//     switch (io) {
//         case 21:
//             system("devmem 0x02304084 32 0x00060083");
//             break;
//         case 22:
//             system("devmem 0x02304090 32 0x00060083");
//             break;
//         default:
//             break;
//     }
// }
// void gpioWrite(int io, int out) {
//     switch (io) {
//         case 21:
//             if (out)
//                 system("devmem 0x480100c 32 0x3");
//             else
//                 system("devmem 0x480100c 32 0x2");
//             break;
//         case 22:
//             if (out)
//                 system("devmem 0x4801010 32 0x3");
//             else
//                 system("devmem 0x4801010 32 0x2");
//             break;
//         default:
//             break;
//     }
// }

// pin 21 ESP_EN 0x02304084
// pin 22 ESP_BOOT 0x02304090

void gpioSetMode(int io, int mode)
{
    system("mem w 0x09040004 0x40004088");
}
void gpioWrite(int io, int out)
{
    switch (io) {
        case 21:
            if (out) {
                system("mem s 0x09040000 1 30 31");
                printf("21 --- 1\n");
            } else {
                system("mem s 0x09040000 0 30 31");
                printf("21 --- 0\n");
            }

            break;
        case 22:
            if (out) {
                system("mem s 0x09040000 1 3 4");
                printf("22 --- 1\n");
            }
            else {
                system("mem s 0x09040000 0 3 4");
                printf("22 --- 0\n");
            }

            break;
        default:
            break;
    }
}

void gpioTerminate()
{
}
int gpioInitialise()
{
    return 0;
}
int serReadByte(int serial)
{
    return PI_SER_READ_NO_DATA;
}
int serOpen(const char *dev, int speed, int flage)
{
    return -1;
}
void gpioDelay(long long ns)
{
    usleep(ns);
}