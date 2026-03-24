#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <stdint.h>
#include <mosquitto.h>

struct mosquitto *mqtt_init(const char *host, int port);
void publish_channel_json(struct mosquitto *mosq,
                          uint8_t i2c_addr,
                          int channel,
                          uint16_t vbus_raw,
                          uint16_t vsense_raw,
                          uint32_t vpower_raw);
void mqtt_cleanup(struct mosquitto *mosq);

#endif
