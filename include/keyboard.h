#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "kernel.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

void keyboard_init(void);
char keyboard_getchar(void);
bool keyboard_has_input(void);

#endif
