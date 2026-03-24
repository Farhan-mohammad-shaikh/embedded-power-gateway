#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <stdint.h>

int mqtt_init(const char *host, int port);
void mqtt_set_blink_flag(volatile int *flag);

int mqtt_publish_telemetry(uint8_t i2c_addr,int channel,uint16_t vbus_raw,uint16_t vsense_raw,uint32_t vpower_raw);

int mqtt_loop(void);
void mqtt_cleanup(void);

#endif