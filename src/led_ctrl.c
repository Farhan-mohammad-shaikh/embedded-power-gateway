#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "main.h"
#include "led_ctrl.h"

void led_trigger(const char *cmd)
{
    int fd = open("/sys/class/leds/ACT/trigger", O_WRONLY);
    if (fd < 0)
        return;

    write(fd, cmd, strlen(cmd));
    close(fd);
}

void led_brightness(int on)
{
    int fd = open("/sys/class/leds/ACT/brightness", O_WRONLY);
    if (fd < 0)
        return;

    if (on)
    {
        write(fd, "1", 1);
    }
    else
    {
        write(fd, "0", 1);
    }

    close(fd);
}

void *blink_tread(void *arg)
{
    (void)arg;

    led_trigger("none");
    while (running)
    {
        if (blink_enabled)
        {
            led_brightness(1);
            usleep(200000);
            led_brightness(0);
            usleep(200000);
        }
        else
        {
            led_brightness(0);
            usleep(100000);
        }
    }
    led_trigger("mmc0");
    return NULL;
}
