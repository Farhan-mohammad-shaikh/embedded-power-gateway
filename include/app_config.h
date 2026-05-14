#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#define MQTT_TOPIC "lab/led/control"
#define MQTT_TELEMETRY_TOPIC "lab/pac"

#define PAC1944_DEV_0 "/sys/bus/i2c/devices/1-0010"
#define PAC1944_DEV_1 "/sys/bus/i2c/devices/1-0011"
#define PAC1944_DEV_2 "/sys/bus/i2c/devices/1-0012"

#define FS_VBUS_VOLTS    9.0
#define FS_VSENSE_VOLTS  0.1
#define ADC_DENOM        65536.0
#define RSENSE_OHMS      1.0

#endif
