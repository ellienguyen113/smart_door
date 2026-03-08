
#pragma once
#include "driver/gpio.h"


void init_keypad();
char scan_keypad(void);
char get_key_buffered(void);