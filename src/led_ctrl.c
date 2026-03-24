#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include "led_ctrl.h"

static char led_path[256];
static pthread_t led_thread;
static volatile int *blink_enabled = NULL;
static volatile int *running = NULL;
static int thread_started = 0;

static void led_write(int value)
{
    int fd = open(led_path, O_WRONLY);

    if (fd < 0) {
        return;
    }

    if (value) {
        write(fd, "1", 1);
    } else {
        write(fd, "0", 1);
    }

    close(fd);
}

static void *blink_thread(void *arg)
{
    (void)arg;

    while (*running) {
        if (*blink_enabled) {
            led_write(1);
            usleep(200000);
            led_write(0);
            usleep(200000);
        } else {
            led_write(0);
            usleep(100000);
        }
    }

    led_write(0);
    return NULL;
}

int led_ctrl_init(const char *path)
{
    if (path == NULL) {
        return -1;
    }

    memset(led_path, 0, sizeof(led_path));
    strncpy(led_path, path, sizeof(led_path) - 1);

    return 0;
}

int led_ctrl_start(volatile int *blink_flag, volatile int *running_flag)
{
    if (blink_flag == NULL || running_flag == NULL) {
        return -1;
    }

    blink_enabled = blink_flag;
    running = running_flag;

    if (pthread_create(&led_thread, NULL, blink_thread, NULL) != 0) {
        return -1;
    }

    thread_started = 1;
    return 0;
}

void led_ctrl_stop(void)
{
    if (thread_started) {
        pthread_join(led_thread, NULL);
        thread_started = 0;
    }
}

void led_ctrl_cleanup(void)
{
    led_write(0);
}