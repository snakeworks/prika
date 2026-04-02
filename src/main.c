#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEBUG true
#define PORT 8080
#define MAX_CLIENTS 1000
#define DEBUG_TOKEN "dev"
#define NEW_CONN_MSG                                                           \
  "Welcome! Type '/connect <token>' to authorize and start chatting\n"

typedef struct {
  int32_t fd;
  bool authorized;
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

char *generate_token() {
  if (DEBUG) {
    return DEBUG_TOKEN;
  } else {
    // TODO : Generate token
    return "";
  }
}

void broadcast_message(Server *s, Client *c, char *message) {
  pthread_mutex_lock(&s->mutex);

  for (size_t i = 0; i < s->max_clients; i++) {
    if (s->clients[i].fd > 0) {
      send(s->clients[i].fd, message, strlen(message), 0);
    }
  }

  pthread_mutex_unlock(&s->mutex);
}

void *handle_client(void *args) {
  ClientThreadArgs *ctargs = (ClientThreadArgs *)args;
  Server *server = ctargs->server;
  Client *client = ctargs->client;
  free(ctargs);

  send(client->fd, NEW_CONN_MSG, strlen(NEW_CONN_MSG), 0);

  while (true) {
    char buffer[256] = {0};
    uint32_t bytes = recv(client->fd, buffer, 256, 0);

    if (bytes <= 0) {
      break;
    }

    broadcast_message(server, client, buffer);

    printf("INFO: Client (%d) sent: %s", client->fd, buffer);
  }

  close(client->fd);
  printf("INFO: Client (%d) disconnected\n", client->fd);

  // Clean up
  client->fd = 0;
  pthread_exit(&client->thread);
}

void accept_client(Server *s, int client_fd) {
  Client *client;
  for (size_t i = 0; i < s->max_clients; i++) {
    if (s->clients[i].fd == 0) {
      client = &s->clients[i];
      break;
    }
  }

  if (client == NULL) {
    printf("INFO: New client attempted to join, but server is full\n");
    close(client_fd);
    return;
  }

  client->fd = client_fd;

  ClientThreadArgs *args = malloc(sizeof(ClientThreadArgs));
  args->client = client;
  args->server = s;
  pthread_create(&client->thread, NULL, handle_client, (void *)args);
}

void server_init(Server *s) {
  s->fd = socket(AF_INET, SOCK_STREAM, 0);
  int32_t opt = 1;
  setsockopt(s->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in addr = {AF_INET, htons(PORT), 0};
  int b = bind(s->fd, (struct sockaddr *)&addr, sizeof(addr));
  if (b != 0) {
    printf("ERROR: Failed to bind port\n");
    exit(1);
  }

  s->clients = calloc(s->max_clients, sizeof(Client));

  s->token = generate_token();

  int l = listen(s->fd, 10);
  if (l != 0) {
    printf("ERROR: Failed to listen\n");
    exit(1);
  }

  pthread_mutex_init(&s->mutex, NULL);

  printf("INFO: Listening on port '%d'\n", PORT);
  printf("INFO: Token is '%s'\n", s->token);

  while (true) {
    int client_fd = accept(s->fd, 0, 0);
    accept_client(s, client_fd);
  }

  pthread_mutex_destroy(&s->mutex);
}

int main(void) {
  Server server = {.max_clients = MAX_CLIENTS, .port = PORT};

  server_init(&server);

  return 0;
}
