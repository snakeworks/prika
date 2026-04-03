#ifndef CHAT_CMD_H
#define CHAT_CMD_H

#include "server.h"

#define MAX_RESULT 512

void cmd_exec(Server *s, Client *c, char *input, char result[MAX_RESULT]);
void cmd_nick(Server *s, Client *c, uint32_t argc, char **argv, char result[MAX_RESULT]);

#endif
