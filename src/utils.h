#ifndef UTILS_H
#define UTILS_h
#include <stdbool.h>

int string_to_argv(char *argv[], int size, char *str);
void concat_argv(char *dest, int in_argc, char **in_argv);
bool is_integer(const char *str);
#endif
