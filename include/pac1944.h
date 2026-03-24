#ifndef PAC1944_H
#define PAC1944_H

#include <stdint.h>

int pac_open(const char *dev);
void pac_close(int fd);

int pac_set_slave(int fd, uint8_t addr);
int pac_refresh_g(int fd);

int pac_read_channel1(int fd,uint8_t addr,uint16_t *vbus,uint16_t *vsense,uint32_t *vpower);

int pac_read_channel3(int fd,uint8_t addr,uint16_t *vbus,uint16_t *vsense,uint32_t *vpower);

#endif