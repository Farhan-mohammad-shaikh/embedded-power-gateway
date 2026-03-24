#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdint.h>

#define I2C_DEV "/dev/i2c-1"
#define MQTT_TOPIC "lab/led/control"
#define MQTT_TELEMETRY_TOPIC "lab/pac"

/* for channel 1, pac 0x10 and pac 0x11 is in channel 1 */
#define PAC_REFRESH_CMD 0x00
#define REG_VBUS1       0x07
#define REG_VSENSE1     0x0B
#define REG_VPOWER1     0x17

/* for channel 3, pac 0x12 is in channel 3 */
#define REG_VBUS3       0x09
#define REG_VSENSE3     0x0D
#define REG_VPOWER3     0x19

/* scaling constants according to datasheet formula */
#define FS_VBUS_VOLTS    9.0
#define FS_VSENSE_VOLTS  0.1
#define ADC_DENOM        65536.0
#define RSENSE_OHMS      1.0

extern uint8_t pac_address[2];

#endif
