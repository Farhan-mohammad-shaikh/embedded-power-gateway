#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "app_config.h"
#include "pac1944.h"
#include "mqtt_client.h"
#include "led_ctrl.h"
#include "logger.h"


static volatile sig_atomic_t running = 1;
static volatile int blink_enabled = 0;


static void on_sigint(int sig)
{
    (void)sig;
    running = 0;
}


int main(void)
{
    int fd;
    uint16_t vbus;
    uint16_t vsense;
    uint32_t vpower;
    int i;

    signal(SIGINT, on_sigint);

    /* Open I2C device */
    fd = pac_open(I2C_DEV);
    if (fd < 0) {
        perror("open i2c");
        return 1;
    }

    /*
     * Set one slave initially so refresh command has a valid selected device.
     */
    if (pac_set_slave(fd, pac_address[0]) != 0) {
        pac_close(fd);
        return 1;
    }

    /* ---------- Init MQTT ---------- */
    if (mqtt_init(MQTT_BROKER_HOST, MQTT_BROKER_PORT) != 0) {
        pac_close(fd);
        return 1;
    }

    mqtt_set_blink_flag(&blink_enabled);

    /* ---------- Start LED blink thread ---------- */
    if (led_ctrl_init(LED_BRIGHTNESS_PATH) != 0) {
        log_error("Failed to initialize LED control");
        mqtt_cleanup();
        pac_close(fd);
        return 1;
    }

    if (led_ctrl_start(&blink_enabled, &running) != 0) {
        log_error("Failed to create blink thread");
        mqtt_cleanup();
        pac_close(fd);
        return 1;
    }


    while (running) {
        /* Global snapshot / refresh */
        if (pac_refresh_g(fd) != 0) {
            usleep(MAIN_LOOP_DELAY_US);
            mqtt_loop();
            continue;
        }

        /* Give time for measurement registers to settle */
        usleep(PAC_REFRESH_DELAY_US);

        /*
         * Read channel 1 from PAC devices in pac_address[]
         */
        for (i = 0; i < 2; i++) {
            if (pac_read_channel1(fd, pac_address[i], &vbus, &vsense, &vpower) == 0) {
                mqtt_publish_telemetry(pac_address[i], 1, vbus, vsense, vpower);
            }
        }

        /*
         * Read channel 3 from device at address 0x12
         */
        if (pac_read_channel3(fd, PAC_ADDR_CH3, &vbus, &vsense, &vpower) == 0) {
            mqtt_publish_telemetry(PAC_ADDR_CH3, 3, vbus, vsense, vpower);
        }

        
        mqtt_loop();

        /* Sample period */
        usleep(MAIN_LOOP_DELAY_US);
    }

    /* ---------- Clean shutdown ---------- */
    led_ctrl_stop();
    led_ctrl_cleanup();

    pac_close(fd);
    mqtt_cleanup();

    return 0;
}