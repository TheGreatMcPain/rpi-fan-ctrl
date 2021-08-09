#ifndef UNIX_SOCKET_H
#define UNIX_SOCKET_H
int connect_unix_sock(const char* socket_path, int sock_type);
int bind_unix_sock(const char* socket_path, int sock_type);
int listen_accept_unix_sock(int serverfd);
#endif
