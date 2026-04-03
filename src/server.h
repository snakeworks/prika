#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

#define DEBUG true
#define PORT 8080
#define MAX_CLIENTS 1000
#define DEBUG_TOKEN "dev"
#define NEW_CONN_MSG "Welcome!\n"

typedef struct {
  int32_t fd;
  bool authorized;
  char *nickname;
  pthread_t thread;
} Client;

typedef struct {
  int32_t fd;
  uint16_t port;
  char *token;
  uint32_t max_clients;
  Client *clients;
  pthread_mutex_t mutex;
} Server;

typedef struct {
  Server *server;
  Client *client;
} ClientThreadArgs;

char *generate_token();
void broadcast_message(Server *s, char *message);
void accept_client(Server *s, int client_fd);
void server_init(Server *s);

#endif
