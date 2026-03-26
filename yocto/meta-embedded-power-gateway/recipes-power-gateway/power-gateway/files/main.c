#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <mosquitto.h>

#include "main.h"
#include "app_config.h"
#include "pac1944.h"
#include "led_ctrl.h"
#include "mqtt_client.h"
#include "logger.h"

volatile sig_atomic_t running = 1;
volatile sig_atomic_t blink_enabled = 0;

void on_sigint(int signal)
{
    (void)signal;
    running = 0;
}

int main(void)
{
    pthread_t led_thread;
    struct mosquitto *mosq;

    uint8_t raw[2];
    uint8_t pow[4];

    uint8_t raw3[2];
    uint8_t pow3[4];

    signal(SIGINT, on_sigint);

    mosquitto_lib_init();

    int fd = open_i2c_device(I2C_DEV);

    if (fd < 0)
    {
        perror("open i2c");
        return 1;
    }

    mosq = mqtt_init("192.168.178.23", 1883);
    if (mosq == NULL)
    {
        close_i2c_device(fd);
        mosquitto_lib_cleanup();
        return 1;
    }

    if (pthread_create(&led_thread, NULL, blink_tread, NULL) != 0)
    {
        fprintf(stderr, "Failed to create blink thread\n");
        mqtt_cleanup(mosq);
        close_i2c_device(fd);
        return 1;
    }

    while (running)
    {
        if (pac_refresh_g(fd) != 0)
        {
            usleep(200000);
            continue;
        }

        usleep(5000);

        for (int i = 0; i < 2; i++)
        {
            i2c_set_slave(fd, pac_address[i]);

            pac_read_reg(fd, REG_VBUS1, raw, 2);
            uint16_t vbus = (raw[0] << 8) | raw[1];

            pac_read_reg(fd, REG_VSENSE1, raw, 2);
            uint16_t vsense = (raw[0] << 8) | raw[1];

            pac_read_reg(fd, REG_VPOWER1, pow, 4);
            uint32_t vpower = (pow[0] << 24) | (pow[1] << 16) | (pow[2] << 8) | pow[3];

            publish_channel_json(mosq, pac_address[i], 1, vbus, vsense, vpower);
        }

        i2c_set_slave(fd, 0x12);

        pac_read_reg(fd, REG_VBUS3, raw3, 2);
        uint16_t vbus3 = (raw3[0] << 8) | raw3[1];

        pac_read_reg(fd, REG_VSENSE3, raw3, 2);
        uint16_t vsense3 = (raw3[0] << 8) | raw3[1];

        pac_read_reg(fd, REG_VPOWER3, pow3, 4);
        uint32_t vpower3 = (pow3[0] << 24) | (pow3[1] << 16) | (pow3[2] << 8) | pow3[3];

        publish_channel_json(mosq, 0x12, 3, vbus3, vsense3, vpower3);
        usleep(200000);

        mosquitto_loop(mosq, 0, 1);
    }

    pthread_join(led_thread, NULL);
    close_i2c_device(fd);

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
