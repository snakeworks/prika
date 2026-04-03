#include "commands.h"
#include "utils/str_utils.h"
#include <stdio.h>
#include <string.h>

void cmd_exec(Server *s, Client *c, char *input) {
  uint32_t argc = 0;
  char *argv[8] = {0};

  char *tok = strtok(input, " ") + 1;
  while (tok != NULL) {
    argv[argc] = tok;
    argc++;
    tok = strtok(NULL, " ");
  }

  argv[argc] = NULL;
  char *cmd = argv[0];

  if (strcmp(cmd, "nick") == 0) {
    cmd_nick(s, c, argc, argv);
  }
}

void cmd_nick(Server *s, Client *c, uint32_t argc, char **argv) {
  str_trim(argv[1]);
  argv[1][MAX_NICK] = '\0'; // idk if necessary?
  sprintf(c->nickname, "%s", argv[1]);
}
