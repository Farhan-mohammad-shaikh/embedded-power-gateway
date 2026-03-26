#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "pac1944.h"

int open_i2c_device(const char *device)
{
    return open(device, O_RDWR);
}

void close_i2c_device(int fd)
{
    close(fd);
}

int i2c_set_slave(int fd, uint8_t addr)
{
    if (ioctl(fd, I2C_SLAVE, addr) < 0)
    {
        fprintf(stderr, "I2C_SLAVE 0x%02X failed: %s\n", addr, strerror(errno));
        return -1;
    }
    return 0;
}

int pac_refresh_g(int fd)
{
    if (i2c_set_slave(fd, 0x00) != 0)
        return -1;

    uint8_t cmd = 0x1E;
    if (write(fd, &cmd, 1) != 1)
    {
        fprintf(stderr, "REFRESH_G write failed: %s\n", strerror(errno));
        return -2;
    }
    return 0;
}

int pac_read_reg(int fd, uint8_t reg, uint8_t *buf, size_t len)
{
    if (write(fd, &reg, 1) != 1)
    {
        fprintf(stderr, "Write reg 0x%02X failed: %s\n", reg, strerror(errno));
        return -1;
    }

    int r = read(fd, buf, len);
    if (r != (int)len)
    {
        fprintf(stderr, "Read reg 0x%02X failed: got %d/%zu (%s)\n",
                reg, r, len, strerror(errno));
        return -2;
    }
    return 0;
}
