#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pac1944.h"

static int read_u32_from_file(const char *path, uint32_t *value)
{
    FILE *fp;
    char buf[64];
    char *endptr;
    unsigned long tmp;

    fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Failed to open %s: %s\n", path, strerror(errno));
        return -1;
    }

    if (!fgets(buf, sizeof(buf), fp)) {
        fprintf(stderr, "Failed to read %s\n", path);
        fclose(fp);
        return -2;
    }

    fclose(fp);

    errno = 0;
    tmp = strtoul(buf, &endptr, 10);

    if (errno != 0 || endptr == buf) {
        fprintf(stderr, "Invalid numeric value in %s\n", path);
        return -3;
    }

    *value = (uint32_t)tmp;
    return 0;
}

int pac1944_read_sysfs(const char *device_path, struct pac1944_sample *sample)
{
    char path[256];
    uint32_t value;

    if (!device_path || !sample)
        return -1;

    snprintf(path, sizeof(path), "%s/vbus_raw", device_path);
    if (read_u32_from_file(path, &value) != 0)
        return -2;
    sample->vbus_raw = (uint16_t)value;

    snprintf(path, sizeof(path), "%s/vsense_raw", device_path);
    if (read_u32_from_file(path, &value) != 0)
        return -3;
    sample->vsense_raw = (uint16_t)value;

    snprintf(path, sizeof(path), "%s/vpower_raw", device_path);
    if (read_u32_from_file(path, &value) != 0)
        return -4;
    sample->vpower_raw = value;

    return 0;
}
