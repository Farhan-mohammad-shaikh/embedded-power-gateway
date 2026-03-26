#ifndef LED_CTRL_H
#define LED_CTRL_H

void led_trigger(const char *cmd);
void led_brightness(int on);
void *blink_tread(void *arg);

#endif
