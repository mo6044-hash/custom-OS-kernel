#ifndef SHELL_H
#define SHELL_H

#include "kernel.h"

#define SHELL_MAX_INPUT 256
#define SHELL_MAX_ARGS 16

void shell_run(void);
void shell_execute(const char* input);

#endif
