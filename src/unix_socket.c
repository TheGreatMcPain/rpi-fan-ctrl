#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#include "unix_socket.h"

// const char* socket_path = "server.sock";

int create_unix_sock(const char* socket_path, int sock_type, struct sockaddr_un *addr) {
  int sockfd;

  // Setup socket address structure
  memset(addr, 0, sizeof(struct sockaddr_un));
  addr->sun_family = AF_UNIX;

  if (strlen(socket_path) < sizeof(addr->sun_path)) {
    strncpy(addr->sun_path, socket_path, sizeof(addr->sun_path) - 1);
  } else {
    errno = ENAMETOOLONG;
    return -1;
  }

  // Create socket
  sockfd = socket(PF_UNIX, sock_type, 0);
  if (sockfd < 0) {
    return -1;
  }

  return sockfd;
}

int connect_unix_sock(const char* socket_path, int sock_type) {
  struct sockaddr_un sock_addr;
  int sockfd;

  sockfd = create_unix_sock(socket_path, sock_type, &sock_addr);
  if (sockfd < 0) {
    return -1;
  }

  if (connect(sockfd, (const struct sockaddr *)&sock_addr, sizeof(struct sockaddr_un)) < 0) {
    int saved_errno = errno;
    close(sockfd);
    errno = saved_errno;
    return -1;
  }

  return sockfd;
}

int bind_unix_sock(const char* socket_path, int sock_type) {
  struct sockaddr_un sock_addr;
  int sockfd;

  sockfd = create_unix_sock(socket_path, sock_type, &sock_addr);
  if (sockfd < 0) {
    return -1;
  }

  if (bind(sockfd, (const struct sockaddr *)&sock_addr, sizeof(struct sockaddr_un)) < 0) {
    int saved_errno = errno;
    close(sockfd);
    errno = saved_errno;
    return -1;
  }

  return sockfd;
}

// listen() and accept() client connection and return clientfd.
int listen_accept_unix_sock(int serverfd) {
  struct sockaddr_un client_addr;
  socklen_t client_length = sizeof(client_addr);
  int clientfd;

  // Listen for incoming connections
  if (listen(serverfd, SOMAXCONN) < 0) {
    return -1;
  }

  // Accept client connection
  clientfd = accept(serverfd, (struct sockaddr *)&client_addr, &client_length);
  if (clientfd < 0) {
    int saved_errno = errno;
    close(clientfd);
    errno = saved_errno;
    return -1;
  }

  return clientfd;
}
