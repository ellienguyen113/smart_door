<<<<<<< HEAD

#pragma once
#include "driver/gpio.h"


void init_keypad();
=======
#pragma once

/* Public functions for keypad module */

void init_keypad(void);
>>>>>>> 332e6a0c282f0212a4c34c8fa3b2d8734112cb9d
char scan_keypad(void);
char get_key_buffered(void);