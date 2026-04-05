#include "server.h"

int main(void) {
  Server server = {
    .max_clients = 1000,
    .port = 8080,
    .password = "test"
  };

  server_init(&server);

  return 0;
}
