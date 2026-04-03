#include "str_utils.h"

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
