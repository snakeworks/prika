#include "commands.h"
#include "utils/str_utils.h"
#include <stdio.h>
#include <string.h>

void cmd_exec(Server *s, Client *c, char *input, char result[MAX_RESULT]) {
  uint32_t argc = 0;
  char *argv[8] = {0};

  char *tok = strtok(input, " ") + 1;
  while (tok != NULL) {
    argv[argc] = tok;
    argc++;
    tok = strtok(NULL, " ");
  }

  if (argc == 0) {
    argc = 1;
  }

  argv[argc] = NULL;
  char *cmd = argv[0];
  str_trim(cmd);

  if (strcmp(cmd, "nick") == 0) {
    cmd_nick(s, c, argc, argv, result);
  } else if (strcmp(cmd, "me") == 0) {
    cmd_me(s, c, argc, argv, result);
  } else {
    sprintf(result, "Unknown command '%s'\n", cmd);
  }
}

void cmd_nick(Server *s, Client *c, uint32_t argc, char **argv, char result[MAX_RESULT]) {
  if (argc != 2) {
    sprintf(result, "You must provide a nickname. Usage: /nick <nickname>\n");
    return;
  }
  if (str_is_empty(argv[1], strlen(argv[1]))) {
    sprintf(result, "Nickname cannot be empty. Usage: /nick <nickname>\n");
    return;
  }
  str_trim(argv[1]);
  argv[1][MAX_NICK] = '\0'; // idk if necessary?
  sprintf(c->nickname, "%s", argv[1]);
  sprintf(result, "Your nickname is now '%s'\n", c->nickname);
}

void cmd_me(Server *s, Client *c, uint32_t argc, char **argv, char result[MAX_RESULT]) {
  sprintf(result, "Nickname: %s\nRole: %s\n", c->nickname, role_to_str(c->role));
}
