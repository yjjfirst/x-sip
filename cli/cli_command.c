#include "cli.h"
int get_command_impl(char *buf)
{
}

int (*get_command)(char *buf) = get_command_impl;
