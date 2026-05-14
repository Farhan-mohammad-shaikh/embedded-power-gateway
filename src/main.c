#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <systemd/sd-daemon.h>
#include <mosquitto.h>

#include "main.h"
#include "app_config.h"
#include "pac1944.h"
#include "mqtt_client.h"
#include "led_ctrl.h"

volatile sig_atomic_t running = 1;
volatile sig_atomic_t blink_enabled = 0;

void on_sigint(int signal)
{
    (void)signal;
    running = 0;
}

static void process_device(struct mosquitto *mosq,
                           const char *path,
                           int channel)
{
    struct pac1944_sample sample;

    if (pac1944_read_sysfs(path, &sample) != 0) {
        fprintf(stderr, "Failed to read %s\n", path);
        return;
    }

    publish_channel_json(
            mosq,
            channel,
            sample.vbus_raw,
            sample.vsense_raw,
            sample.vpower_raw);
}

int main(void)
{
    pthread_t led_thread;
    struct mosquitto *mosq;

    signal(SIGINT, on_sigint);

    mosquitto_lib_init();

    mosq = mqtt_init("192.168.178.23", 1883);
    sd_notify(0, "READY=1");
    if (!mosq) {
        fprintf(stderr, "MQTT init failed\n");
        return 1;
    }

    if (pthread_create(
                &led_thread,
                NULL,
                blink_tread,
                NULL) != 0) {

        fprintf(stderr, "Failed to create LED thread\n");

        mqtt_cleanup(mosq);
        return 1;
    }

    while (running)
    {
        process_device(mosq, PAC1944_DEV_0, 0);
        process_device(mosq, PAC1944_DEV_1, 1);
        process_device(mosq, PAC1944_DEV_2, 2);

        mosquitto_loop(mosq, 0, 1);
        sd_notify(0, "WATCHDOG=1");
        usleep(200000);

    }

    pthread_join(led_thread, NULL);

    mqtt_cleanup(mosq);

    mosquitto_lib_cleanup();

    return 0;
}
