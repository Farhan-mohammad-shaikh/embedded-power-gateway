#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "app_config.h"
#include "pac1944.h"

static int pac_read_reg(int fd, uint8_t reg, uint8_t *buf, size_t len)
{
    if (write(fd, &reg, 1) != 1) {
        perror("i2c write reg addr");
        return -1;
    }

    if (read(fd, buf, len) != (ssize_t)len) {
        perror("i2c read reg data");
        return -1;
    }

    return 0;
}

int pac_open(const char *dev)
{
    return open(dev, O_RDWR);
}

void pac_close(int fd)
{
    if (fd >= 0) {
        close(fd);
    }
}

int pac_set_slave(int fd, uint8_t addr)
{
    if (ioctl(fd, I2C_SLAVE, addr) < 0) {
        perror("ioctl(I2C_SLAVE)");
        return -1;
    }

    return 0;
}

int pac_refresh_g(int fd)
{
    uint8_t reg = REG_REFRESH_G;

    if (write(fd, &reg, 1) != 1) {
        perror("pac_refresh_g");
        return -1;
    }

    return 0;
}

int pac_read_channel1(int fd,uint8_t addr,uint16_t *vbus,uint16_t *vsense, uint32_t *vpower)
{
    uint8_t raw[2];
    uint8_t pow[4];

    if (pac_set_slave(fd, addr) != 0) {
        return -1;
    }

    if (pac_read_reg(fd, REG_VBUS1, raw, 2) != 0) {
        return -1;
    }
    *vbus = ((uint16_t)raw[0] << 8) | raw[1];

    if (pac_read_reg(fd, REG_VSENSE1, raw, 2) != 0) {
        return -1;
    }
    *vsense = ((uint16_t)raw[0] << 8) | raw[1];

    if (pac_read_reg(fd, REG_VPOWER1, pow, 4) != 0) {
        return -1;
    }
    *vpower = ((uint32_t)pow[0] << 24) |
              ((uint32_t)pow[1] << 16) |
              ((uint32_t)pow[2] << 8)  |
               (uint32_t)pow[3];

    return 0;
}

int pac_read_channel3(int fd,uint8_t addr,uint16_t *vbus,uint16_t *vsense,uint32_t *vpower)
{
    uint8_t raw[2];
    uint8_t pow[4];

    if (pac_set_slave(fd, addr) != 0) {
        return -1;
    }

    if (pac_read_reg(fd, REG_VBUS3, raw, 2) != 0) {
        return -1;
    }
    *vbus = ((uint16_t)raw[0] << 8) | raw[1];

    if (pac_read_reg(fd, REG_VSENSE3, raw, 2) != 0) {
        return -1;
    }
    *vsense = ((uint16_t)raw[0] << 8) | raw[1];

    if (pac_read_reg(fd, REG_VPOWER3, pow, 4) != 0) {
        return -1;
    }
    *vpower = ((uint32_t)pow[0] << 24) |
              ((uint32_t)pow[1] << 16) |
              ((uint32_t)pow[2] << 8)  |
               (uint32_t)pow[3];

    return 0;
}