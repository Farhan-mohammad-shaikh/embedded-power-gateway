#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdint.h>

/* I2C device node */
#define I2C_DEV "/dev/i2c-1"

/* MQTT broker settings */
#define MQTT_BROKER_HOST "192.168.178.23"
#define MQTT_BROKER_PORT 1883

/* MQTT topics */
#define MQTT_TOPIC           "pac/cmd"
#define MQTT_TELEMETRY_TOPIC "pac/telemetry"

/* PAC194x I2C addresses on the bus */
extern const uint8_t pac_address[2];
#define PAC_ADDR_CH3 0x12


#define REG_REFRESH_G   0x00

#define REG_VBUS1       0x07
#define REG_VSENSE1     0x0B
#define REG_VPOWER1     0x17

#define REG_VBUS3       0x09
#define REG_VSENSE3     0x0D
#define REG_VPOWER3     0x1F


#define ADC_DENOM        65535.0
#define FS_VBUS_VOLTS    32.0
#define FS_VSENSE_VOLTS  0.1
#define RSENSE_OHMS      0.01

/* LED sysfs path */
#define LED_BRIGHTNESS_PATH "/sys/class/leds/led0/brightness"

/* Timing */
#define PAC_REFRESH_DELAY_US 5000
#define MAIN_LOOP_DELAY_US   200000

#endif