#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client.h"
#include "gpio_pin.h"
#include "server.h"
#include "utils.h"

#define DEFAULT_MIN 40
#define DEFAULT_MAX 50

// Protos
void help(char *argv[]);

int main(int argc, char *argv[]) {
  bool foreground = false;
  bool start_server = false;
  int default_min_temp = -1;
  int default_max_temp = -1;
  int opt;
  char socket_path[256];
  char *new_socket_path = getenv("RPIFANCTRL_SOCK");

  // Set socket_path;
  memset(socket_path, 0, 256);
  if (new_socket_path == NULL) {
    strcpy(socket_path, "/tmp/rpi-fan-ctrl.sock");
  } else {
    strcpy(socket_path, new_socket_path);
  }

  // Server arguments
  while ((opt = getopt(argc, argv, "hdfu:l:")) != -1) {
    switch (opt) {
      case 'h':
        help(argv);
        exit(1);
      case 'd':
        start_server = true;
        break;
      case 'f':
        foreground = true;
        break;
      case 'u':
        if (!is_integer(optarg)) {
          printf("'-u' Input must be an integer.\n");
          exit(1);
        }
        if (strlen(optarg) > 2 && strlen(optarg) < 2) {
          printf("'-u' Input must be 2 digits.\n");
          exit(1);
        }
        sscanf(optarg, "%2d", &default_max_temp);
        break;
      case 'l':
        if (!is_integer(optarg)) {
          printf("'-l' Input must be an integer.\n");
          exit(1);
        }
        if (strlen(optarg) > 2 && strlen(optarg) < 2) {
          printf("'-l' Input must be 2 digits.\n");
          exit(1);
        }
        sscanf(optarg, "%2d", &default_min_temp);
        break;
      default:
        help(argv);
        exit(1);
    }
  }

  // Prevent server arguments from getting into the client arguments.
  if (!start_server && foreground) {
    printf("'-f' requires '-d'.\n");
    exit(1);
  }
  if (!start_server && default_max_temp != -1) {
    printf("'-u' requires '-d'.\n");
    exit(1);
  }
  if (!start_server && default_min_temp != -1) {
    printf("'-l' requires '-d'.\n");
    exit(1);
  }

  if (start_server) {
    if (default_max_temp == -1) {
      default_max_temp = DEFAULT_MAX;
    }
    if (default_min_temp == -1) {
      default_min_temp = DEFAULT_MIN;
    }

    server(socket_path, foreground, default_max_temp, default_min_temp);
    return 0;  // not really needed, but just to make sure we exit here.
  }

  client(socket_path, argc, argv);
}

void help(char *argv[]) {
  printf("\nUsage: %s -h,-u <temp> -l <temp>\n\n", argv[0]);
  printf("    '-h':        Display this help text.\n");
  printf("    '-d':        Start daemon.\n\n");
  printf("  Options that require '-d':\n");
  printf("    '-f':        Start daemon in foreground. (requires -d)\n");
  printf("    '-u <temp>': The Fan will switch on if the\n");
  printf("                 CPU tempature goes above this value.");
  printf("                 (default is %d)\n\n", DEFAULT_MAX);
  printf("    '-l <temp>': The Fan will turn off if the\n");
  printf("                 CPU tempature goes below this value.");
  printf("                 (default is %d)\n\n", DEFAULT_MIN);
}
