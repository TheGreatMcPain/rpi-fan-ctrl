#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "unix_socket.h"
#include "utils.h"

void client(const char *socket_path, int argc, char **argv) {
  int serverfd, n;
  char client_buffer[256];

  // Create socket
  serverfd = connect_unix_sock(socket_path, SOCK_STREAM);
  if (serverfd < 0) {
    perror("connect_unix_sock()");
    exit(1);
  }

  // Convert argv into a single string.
  memset(client_buffer, 0, 256);
  concat_argv(client_buffer, argc, argv);

  // Send message to server.
  n = write(serverfd, client_buffer, 255);
  if (n < 0) {
    perror("write()");
    exit(1);
  }

  // Get data from server and print it.
  memset(client_buffer, 0, 256);
  n = read(serverfd, client_buffer, 255);
  if (n < 0) {
    perror("read()");
    exit(1);
  }

  printf("%s\n", client_buffer);
}
