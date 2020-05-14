#include <ctype.h>
#include <pigpio.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// macros
#define PIN 18

// Protos
void help(char *argv[]);
int getTemp();
bool isStrNum(char str[]);
void stop(int signum);

int main(int argc, char *argv[]) {
  int temp;
  int minTemp = 30;
  int maxTemp = 50;
  int opt;

  while ((opt = getopt(argc, argv, ":hu:l:")) != -1) {
    switch (opt) {
      case 'h':
        help(argv);
        exit(1);
      case 'u':
        if (!isStrNum(optarg)) {
          printf("'-u' Input must be an integer.\n");
          exit(1);
        }
        if (strlen(optarg) > 2 && strlen(optarg) < 2) {
          printf("'-u' Input must be 2 digits.\n");
          exit(1);
        }
        sscanf(optarg, "%2d", &maxTemp);
        break;
      case 'l':
        if (!isStrNum(optarg)) {
          printf("'-l' Input must be an integer.\n");
          exit(1);
        }
        if (strlen(optarg) > 2 && strlen(optarg) < 2) {
          printf("'-l' Input must be 2 digits.\n");
          exit(1);
        }
        sscanf(optarg, "%2d", &minTemp);
        break;
      case ':':
        help(argv);
        exit(1);
      case '?':
        help(argv);
        exit(1);
    }
  }

  if (gpioInitialise() < 0) return -1;

  gpioSetSignalFunc(SIGINT, stop);

  while (true) {
    temp = getTemp();

    if (temp > maxTemp) {
      printf(
          "CPU Temp is %d. FAN is ON (Upper Threshold is %d, Lower Threshold "
          "is %d)\n",
          temp, maxTemp, minTemp);
      gpioPWM(PIN, 255);
    } else if (temp < minTemp) {
      printf(
          "CPU Temp is %d. FAN is OFF (Upper Threshold is %d, Lower Threshold "
          "is %d)\n",
          temp, maxTemp, minTemp);
      gpioPWM(PIN, 0);
    }
    sleep(5);
  }
  return 0;
}

void help(char *argv[]) {
  printf("\nUsage: %s -h,-u <temp> -l <temp>\n\n", argv[0]);
  printf("    '-h':        Display this help text.\n");
  printf("    '-u <temp>': The Fan will switch on if the\n");
  printf("                 CPU tempature goes above this value.");
  printf("                 (default is 50)\n\n");
  printf("    '-l <temp>': The Fan will turn off if the\n");
  printf("                 CPU tempature goes below this value.");
  printf("                 (default is 30)\n\n");
}

void stop(int signum) {
  printf("\nSignal %d caught.\n", signum);
  gpioPWM(PIN, 0);
  gpioTerminate();
  exit(0);
}

int getTemp() {
  FILE *f;
  int temp;
  f = fopen("/sys/class/thermal/thermal_zone0/temp", "r");

  if (fscanf(f, "%2d", &temp)) {
  }

  fclose(f);
  return temp;
}

bool isStrNum(char str[]) {
  while (*str != '\0') {
    if (!isdigit(*str)) {
      return false;
    }
    str++;
  }
  return true;
}
