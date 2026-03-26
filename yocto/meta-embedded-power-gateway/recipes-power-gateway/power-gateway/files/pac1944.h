#ifndef PAC1944_H
#define PAC1944_H

#include <stddef.h>
#include <stdint.h>

int open_i2c_device(const char *device);
void close_i2c_device(int fd);

int i2c_set_slave(int fd, uint8_t addr);
int pac_refresh_g(int fd);
int pac_read_reg(int fd, uint8_t reg, uint8_t *buf, size_t len);

#endif
