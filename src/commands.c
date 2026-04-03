#include "commands.h"
#include <string.h>

void cmd_exec(char *input) {
  char *cmd = "";

  if (strcmp(cmd, "nick") == 0) {
  }
}

void cmd_nick(Server *s, Client *c, uint32_t argc, char **argv) {
  char *cname = argv[0];
}
