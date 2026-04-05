#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

#define DEBUG true
#define PORT 8080
#define MAX_CLIENTS 1000
#define MAX_NICK 64

typedef struct {
  int32_t fd;
  bool authorized;
  char nickname[MAX_NICK];
  pthread_t thread;
} Client;

typedef struct {
  int32_t fd;
  uint16_t port;
  char *password;
  uint32_t max_clients;
  Client *clients;
  pthread_mutex_t mutex;
} Server;

typedef struct {
  Server *server;
  Client *client;
} ClientThreadArgs;

void broadcast_message(Server *s, char *message);
void accept_client(Server *s, int client_fd);
void server_init(Server *s);

#endif
