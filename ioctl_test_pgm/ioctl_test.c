#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_PATH "/dev/my_i2c_device"
#define IOCTL_PRINT_MESSAGE _IO('a', 1)

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // Send an ioctl command
    char *msg = "Hello from ioctl!";
    if (ioctl(fd, IOCTL_PRINT_MESSAGE, msg) < 0) {
        perror("ioctl failed");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}

