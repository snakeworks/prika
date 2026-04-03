#ifndef CHAT_CMD_H
#define CHAT_CMD_H

#include "server.h"

void cmd_exec(Server *s, Client *c, char *input);
void cmd_nick(Server *s, Client *c, uint32_t argc, char **argv);

#endif
