#ifndef MAIN_H
#define MAIN_H

#include <signal.h>

extern volatile sig_atomic_t running;
extern volatile sig_atomic_t blink_enabled;

void on_sigint(int signal);

#endif
