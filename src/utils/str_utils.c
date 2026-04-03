#include "str_utils.h"
#include <string.h>

bool str_is_empty(char *str, size_t len) {
  for (size_t i = 0; i < len; i++) {
    if (str[i] == 0) {
      return true;
    }
    if (isspace(str[i]) == false) {
      return false;
    }
  }
  return true;
}

bool str_starts_with(char *str, char c) {
  if (str == NULL) {
    return false;
  }
  return str[0] == c;
}

void str_trim(char *str) {
  char *end;

  while (isspace((unsigned char)*str)) {
    str++;
  }

  if (*str == 0) {
    return;
  } // All spaces?

  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end)) {
    end--;
  }

  end[1] = '\0';
}
