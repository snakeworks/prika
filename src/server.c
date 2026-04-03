#include "server.h"
#include "commands.h"
#include "utils/str_utils.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

char *generate_token() {
  if (DEBUG) {
    return DEBUG_TOKEN;
  } else {
    // TODO : Generate token
    return "";
  }
}

void broadcast_message(Server *s, char *message) {
  pthread_mutex_lock(&s->mutex);

  for (size_t i = 0; i < s->max_clients; i++) {
    if (s->clients[i].fd > 0) {
      send(s->clients[i].fd, message, strlen(message), 0);
    }
  }

  pthread_mutex_unlock(&s->mutex);
}

static void *handle_client(void *args) {
  ClientThreadArgs *ctargs = (ClientThreadArgs *)args;
  Server *server = ctargs->server;
  Client *client = ctargs->client;
  free(ctargs);

  sprintf(client->nickname, "user_%d", client->fd);
  send(client->fd, NEW_CONN_MSG, strlen(NEW_CONN_MSG), 0);

  while (true) {
    char buffer[256] = {0};
    uint32_t bytes = recv(client->fd, buffer, 256, 0);

    if (bytes <= 0) {
      break;
    }

    if (str_is_empty(buffer, 256)) {
      continue;
    }

    if (str_starts_with(buffer, '/')) {
      printf("INFO: Client (%d) executed command: %s", client->fd, buffer);
      cmd_exec(server, client, buffer);
    } else {
      char *msg;
      asprintf(&msg, "%s> %s", client->nickname, buffer);
      broadcast_message(server, msg);
      free(msg);
      printf("INFO: Client (%d) sent: %s", client->fd, buffer);
    }
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

  struct sockaddr_in addr = {AF_INET, htons(s->port), 0};
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
