#pragma once

/* Public functions for keypad module */

void init_keypad(void);
char scan_keypad(void);
char get_key_buffered(void);