#pragma once

enum {
    PI_OUTPUT
};
#define PI_SER_READ_NO_DATA -1

void gpioDelay(long long ns);

void gpioSetMode(int io, int mode);
void gpioWrite(int io, int out);
void gpioTerminate();
int gpioInitialise();

// int serOpen(const char *dev, int speed, int flage);
// int serReadByte(int serial);