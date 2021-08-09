#ifndef SERVER_H
#define SERVER_H
#include <stdbool.h>

void server(const char *socket_path, bool foreground, int default_max_temp,
            int default_min_temp);
void server_sig_handler(int signum);
#endif
