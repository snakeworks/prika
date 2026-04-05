#include "server.h"

int main(void) {
  Server server = {
    .max_clients = MAX_CLIENTS,
    .port = PORT,
    .password = "test"
  };

  server_init(&server);

  return 0;
}
