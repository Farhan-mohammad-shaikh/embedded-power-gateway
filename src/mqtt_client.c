#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <mosquitto.h>

#include "app_config.h"
#include "mqtt_client.h"

static struct mosquitto *mosq = NULL;
static volatile int *blink_enabled = NULL;

static void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
    (void)obj;

    if (rc == 0) {
        printf("Connected to broker\n");

        if (mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 0) != MOSQ_ERR_SUCCESS) {
            printf("Subscribe failed for topic: %s\n", MQTT_TOPIC);
            fflush(stdout);
            return;
        }

        printf("Subscribed to topic: %s\n", MQTT_TOPIC);
        fflush(stdout);
    } else {
        printf("Connect failed with rc=%d\n", rc);
        fflush(stdout);
    }
}

static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    (void)mosq;
    (void)obj;

    char payload[256];
    int len;

    if (!msg || !msg->payload) {
        return;
    }

    len = msg->payloadlen;
    if (len >= (int)sizeof(payload)) {
        len = (int)sizeof(payload) - 1;
    }

    memcpy(payload, msg->payload, len);
    payload[len] = '\0';

    printf("Received JSON: %s\n", payload);
    fflush(stdout);

    if (blink_enabled == NULL) {
        return;
    }

    if (strstr(payload, "\"cmd\":\"blink_on\"") != NULL) {
        *blink_enabled = 1;
        printf("blink_enabled = 1\n");
        fflush(stdout);
    } else if (strstr(payload, "\"cmd\":\"blink_off\"") != NULL) {
        *blink_enabled = 0;
        printf("blink_enabled = 0\n");
        fflush(stdout);
    } else {
        printf("Unknown command\n");
        fflush(stdout);
    }
}

int mqtt_init(const char *host, int port)
{
    mosquitto_lib_init();

    mosq = mosquitto_new("pac-controller", true, NULL);
    if (mosq == NULL) {
        fprintf(stderr, "Failed to create mosquitto client\n");
        return -1;
    }

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    if (mosquitto_connect(mosq, host, port, 60) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Failed to connect to broker %s:%d\n", host, port);
        mosquitto_destroy(mosq);
        mosq = NULL;
        return -1;
    }

    return 0;
}

void mqtt_set_blink_flag(volatile int *flag)
{
    blink_enabled = flag;
}

int mqtt_publish_telemetry(uint8_t i2c_addr,int channel,uint16_t vbus_raw,uint16_t vsense_raw,uint32_t vpower_raw)
{
    char payload[512];

    double vbus_v      = FS_VBUS_VOLTS * ((double)vbus_raw / ADC_DENOM);
    double vsense_v    = FS_VSENSE_VOLTS * ((double)vsense_raw / ADC_DENOM);
    double current_a   = vsense_v / RSENSE_OHMS;
    double pwr_fsr     = (0.9 / RSENSE_OHMS);
    uint32_t vpower_code = (vpower_raw >> 2);
    double power_w     = pwr_fsr * ((double)vpower_code / 1073741824.0);

    snprintf(payload, sizeof(payload),
             "{"
             "\"i2c_addr\":\"0x%02X\","
             "\"channel\":%d,"
             "\"vbus_raw\":%u,"
             "\"vsense_raw\":%u,"
             "\"vpower_raw\":%u,"
             "\"vbus_V\":%.6f,"
             "\"current_A\":%.6f,"
             "\"power_W\":%.6f"
             "}",
             i2c_addr,
             channel,
             (unsigned)vbus_raw,
             (unsigned)vsense_raw,
             (unsigned)vpower_raw,
             vbus_v,
             current_a,
             power_w);

    if (mosquitto_publish(mosq,NULL,MQTT_TELEMETRY_TOPIC,(int)strlen(payload),payload,0,false) != MOSQ_ERR_SUCCESS) {
        printf("Failed to publish telemetry\n");
        fflush(stdout);
        return -1;
    }

    printf("Published: %s\n", payload);
    fflush(stdout);

    return 0;
}

int mqtt_loop(void)
{
    if (mosq == NULL) {
        return -1;
    }

    return mosquitto_loop(mosq, 0, 1);
}

void mqtt_cleanup(void)
{
    if (mosq != NULL) {
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
        mosq = NULL;
    }

    mosquitto_lib_cleanup();
}