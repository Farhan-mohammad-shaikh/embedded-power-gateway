#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <mosquitto.h>

#include "main.h"
#include "app_config.h"
#include "mqtt_client.h"

static void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
    (void)obj;

    if (rc == 0)
    {
        printf("Connected to broker\n");
        mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 0);
        printf("Subscribed to topic: %s\n", MQTT_TOPIC);
        fflush(stdout);
    }
    else
    {
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

    if (!msg || !msg->payload)
        return;

    len = msg->payloadlen;
    if (len >= (int)sizeof(payload))
        len = sizeof(payload) - 1;

    memcpy(payload, msg->payload, len);
    payload[len] = '\0';

    printf("Received JSON: %s\n", payload);
    fflush(stdout);

    if (strstr(payload, "\"cmd\":\"blink_on\"") != NULL)
    {
        blink_enabled = 1;
        printf("blink_enabled = 1\n");
        fflush(stdout);
    }
    else if (strstr(payload, "\"cmd\":\"blink_off\"") != NULL)
    {
        blink_enabled = 0;
        printf("blink_enabled = 0\n");
        fflush(stdout);
    }
    else
    {
        printf("Unknown command\n");
        fflush(stdout);
    }
}

struct mosquitto *mqtt_init(const char *host, int port)
{
    struct mosquitto *mosq;

    mosq = mosquitto_new("pac-controller", true, NULL);
    if (mosq == NULL)
    {
        fprintf(stderr, "Failed to create mosquitto client\n");
        return NULL;
    }

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    if (mosquitto_connect(mosq, host, port, 60) != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "Failed to connect to broker\n");
        mosquitto_destroy(mosq);
        return NULL;
    }

    return mosq;
}

void publish_channel_json(struct mosquitto *mosq,
                          uint8_t i2c_addr,
                          int channel,
                          uint16_t vbus_raw,
                          uint16_t vsense_raw,
                          uint32_t vpower_raw)
{
    char payload[512];

    double vbus_v   = FS_VBUS_VOLTS   * ((double)vbus_raw   / ADC_DENOM);
    double vsense_v = FS_VSENSE_VOLTS * ((double)vsense_raw / ADC_DENOM);
    double current_a = vsense_v / RSENSE_OHMS;
    double pwr_fsr = (0.9 / RSENSE_OHMS);
    uint32_t vpower_code = (vpower_raw >> 2);
    double power_w = pwr_fsr * ((double)vpower_code / 1073741824.0);

    snprintf(payload, sizeof(payload),
        "{\"i2c_addr\":\"0x%02X\","
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
        power_w
    );

    mosquitto_publish(mosq, NULL, MQTT_TELEMETRY_TOPIC, strlen(payload), payload, 0, false);
}

void mqtt_cleanup(struct mosquitto *mosq)
{
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
}
