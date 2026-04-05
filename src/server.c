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

void broadcast_message(Server *s, char *message) {
  pthread_mutex_lock(&s->mutex);

  for (size_t i = 0; i < s->max_clients; i++) {
    if (s->clients[i].fd > 0 && s->clients[i].authorized) {
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

  char *welcome_msg = "Welcome!\n";
  char *server_passw_msg = "Server is password protected. Enter the password: ";
  char *wrong_passw_msg = "Password is incorrect. Try again: ";
  char *ncmsg = server->password == NULL ? welcome_msg : server_passw_msg;
  send(client->fd, ncmsg, strlen(ncmsg), 0);

  if (server->password == NULL) {
    client->authorized = true;
    printf("INFO: Client (%d) connected and authorized\n", client->fd);
  } else {
    printf("INFO: Client (%d) connected, waiting for authorization\n", client->fd);
  }

  while (true) {
    char buffer[256] = {0};
    uint32_t bytes = recv(client->fd, buffer, 256, 0);

    if (bytes <= 0) {
      break;
    }

    if (str_is_empty(buffer, 256)) {
      continue;
    }

    if (client->authorized == false) {
      // TODO: Maybe a little bit hacky?
      for (size_t i = 0; i < 256; i++) {
        if (isspace(buffer[i])) {
          buffer[i] = '\0';
          break;
        }
      }

      if (strcmp(server->password, buffer) == 0) {
        client->authorized = true;
        send(client->fd, welcome_msg, strlen(welcome_msg), 0);
        printf("INFO: Client (%d) authorized\n", client->fd);
        continue;
      } else {
        send(client->fd, wrong_passw_msg, strlen(wrong_passw_msg), 0);
        continue;
      }
    }

    if (str_starts_with(buffer, '/')) {
      char cmd_result[MAX_RESULT];
      printf("INFO: Client (%d) executed command: %s", client->fd, buffer);
      cmd_exec(server, client, buffer, cmd_result);
      send(client->fd, cmd_result, strlen(cmd_result), 0);
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

  // TODO: Maybe just deallocate the entire client in the future
  client->fd = 0;
  client->authorized = false;
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

  int l = listen(s->fd, 10);
  if (l != 0) {
    printf("ERROR: Failed to listen\n");
    exit(1);
  }

  pthread_mutex_init(&s->mutex, NULL);

  printf("INFO: Listening on port '%d'\n", PORT);

  while (true) {
    int client_fd = accept(s->fd, 0, 0);
    accept_client(s, client_fd);
  }

  pthread_mutex_destroy(&s->mutex);
}
