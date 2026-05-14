#ifndef PAC1944_H
#define PAC1944_H

#include <stdint.h>

struct pac1944_sample {
    uint16_t vbus_raw;
    uint16_t vsense_raw;
    uint32_t vpower_raw;
};

int pac1944_read_sysfs(const char *device_path, struct pac1944_sample *sample);

#endif
