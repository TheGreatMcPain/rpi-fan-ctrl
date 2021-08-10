#include "server.h"

#include <signal.h>
#include <errno.h>
#include <pigpio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "gpio_pin.h"
#include "unix_socket.h"
#include "utils.h"

// The create_unix_sock function (used by bind_unix_sock)
// will error out if socket path is too long. (usually 108 chars)
static char global_socket_path[256];

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_t *thread_id;

static char status_buffer[256];

// temp variables
static int max_temp;
static int min_temp;

int get_temp() {
  // Read tempurature from '/sys/class/thermal'
  FILE *f;
  int temp;
  f = fopen("/sys/class/thermal/thermal_zone0/temp", "r");

  if (f == NULL) {
    return -1;
  }

  if (fscanf(f, "%2d", &temp) == EOF) {
    return -1;
  }

  fclose(f);

  return temp;
}

void *fan_control_thread() {
  int temp = 0;
  bool fan_status = false;

  while (true) {
    temp = get_temp();

    if (temp < 0) {
      perror("get_temp()");
      exit(1);
    }

    pthread_mutex_lock(&lock);
    if (temp > max_temp) {
      fan_status = true;
      gpioPWM(PIN, 255);
    } else if (temp < min_temp) {
      fan_status = false;
      gpioPWM(PIN, 0);
    }

    memset(status_buffer, 0, sizeof(status_buffer));
    sprintf(status_buffer,
            "CPU Temp is %d. Fan is '%s' (Upper Threshold is %d, Lower "
            "Threshold is %d)",
            temp, fan_status ? "ON" : "OFF", max_temp, min_temp);
    pthread_mutex_unlock(&lock);
    sleep(1);
  }
}

void server(const char *socket_path, bool foreground, int default_max_temp,
            int default_min_temp) {
  int serverfd, clientfd, n;
  char client_buffer[256];
  const int argv_size = 10;
  char *client_argv[argv_size];
  int client_argc = 0;

  // Set global_socket_path (used by signal_handler)
  strcpy(global_socket_path, socket_path);

  // Set default temp values
  max_temp = default_max_temp;
  min_temp = default_min_temp;

  // Create socket
  serverfd = bind_unix_sock(socket_path, SOCK_STREAM);
  if (serverfd < 0) {
    perror("bind_unix_sock()");
    exit(1);
  }

  // Fork make parent process exit.
  if (!foreground) {
    pid_t pid = fork();

    if (pid < 0) {
      perror("fork()");
      exit(1);
    } else if (pid > 0) {
      // Fork successful print child pid and exit.
      printf("Forked process: %d\n", pid);
      return;
    }
  }

  // Child will continue here if forked.
 
  // Initialize pigpio library and setup signal handlers
  if (gpioInitialise() < 0) {
    fprintf(stderr, "gpioInitialize() failed\n");
    exit(1);
  }

  gpioSetSignalFunc(SIGINT, server_sig_handler);
  gpioSetSignalFunc(SIGTERM, server_sig_handler);

  // Create fan_control_thread
  thread_id = gpioStartThread(fan_control_thread, NULL);
  if (thread_id == NULL) {
    fprintf(stderr, "Failed to start fan_control_thread via gpioStartThread\n");
    exit(1);
  }

  while (1) {
    clientfd = listen_accept_unix_sock(serverfd);
    if (clientfd < 0) {
      perror("listen_accept_unix_sock()");
      exit(1);
    }

    // If connection is established start communicating.
    memset(client_buffer, 0, 256);
    n = read(clientfd, client_buffer, 255);
    if (n < 0) {
      perror("read()");
      exit(1);
    }

    // Parse the client input into arguments.
    client_argc = string_to_argv(client_argv, argv_size, client_buffer);

    // For now I'm only worried about argc's of 1 and 2.
    if (client_argc == 1 && strncmp(client_argv[0], "status", 6) == 0) {
      pthread_mutex_lock(&lock);
      n = write(clientfd, status_buffer, 255);
      pthread_mutex_unlock(&lock);
      if (n < 0) {
        perror("write()");
        exit(1);
      }
    } else if (client_argc == 2 &&
               strncmp(client_argv[0], "set-upper", 9) == 0) {
      if (is_integer(client_argv[1])) {
        pthread_mutex_lock(&lock);
        sscanf(client_argv[1], "%d", &max_temp);
        pthread_mutex_unlock(&lock);

        // Tell client that we've set upper tempurature threshold
        // (We no longer require client_argv, or client_buffer, so clear it.)
        memset(client_buffer, 0, 256);
        sprintf(client_buffer, "Set upper tempurature threshold to: %d",
                max_temp);
        n = write(clientfd, client_buffer, 255);
        if (n < 0) {
          perror("write()");
          exit(1);
        }
      } else {
        // Tell client that the argument was invalid.
        n = write(clientfd, "Invalid input for command 'set-upper'", 38);
        if (n < 0) {
          perror("write()");
          exit(1);
        }
      }
    } else if (client_argc == 2 &&
               strncmp(client_argv[0], "set-lower", 9) == 0) {
      if (is_integer(client_argv[1])) {
        pthread_mutex_lock(&lock);
        sscanf(client_argv[1], "%d", &min_temp);
        pthread_mutex_unlock(&lock);

        // Tell client that we've set lower tempurature threshold
        memset(client_buffer, 0, 256);
        sprintf(client_buffer, "Set lower tempurature threshold to: %d",
                min_temp);
        n = write(clientfd, client_buffer, 255);
        if (n < 0) {
          perror("write()");
          exit(1);
        }
      } else {
        // Tell client that the argument was invalid.
        n = write(clientfd, "Invalid input for command 'set-lower'", 38);
        if (n < 0) {
          perror("write()");
          exit(1);
        }
      }
    } else {
      // TODO: usage message.
    }
  }
}

void server_sig_handler(int signum) {
  printf("Caught signal %d, coming out...\n", signum);

  if (unlink(global_socket_path) < 0) {
    perror("unlink()");
  }

  // Stop fan_control_thread
  gpioStopThread(thread_id);

  // Turn off the fan.
  gpioPWM(PIN, 0);
  gpioTerminate();

  exit(0);
}
