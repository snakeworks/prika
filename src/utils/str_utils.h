#ifndef STR_UTILS_H
#define STR_UTILS_H

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

bool str_is_empty(char *str, size_t len);
bool str_starts_with(char *str, char c);
void str_trim(char *str);

#endif
