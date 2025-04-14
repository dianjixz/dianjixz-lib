#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/vt.h>

void switch_to_vt(int vt_number) {
    int fd;
    if ((fd = open("/dev/tty0", O_RDWR)) < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (ioctl(fd, VT_ACTIVATE, vt_number) < 0) {
        perror("ioctl VT_ACTIVATE");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (ioctl(fd, VT_WAITACTIVE, vt_number) < 0) {
        perror("ioctl VT_WAITACTIVE");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <VT number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int vt_number = atoi(argv[1]);
    if (vt_number < 1) {
        fprintf(stderr, "Invalid VT number: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    switch_to_vt(vt_number);
    return 0;
}
