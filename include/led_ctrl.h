#ifndef LED_CTRL_H
#define LED_CTRL_H

int led_ctrl_init(const char *path);
int led_ctrl_start(volatile int *blink_flag, volatile int *running_flag);
void led_ctrl_stop(void);
void led_ctrl_cleanup(void);

#endif