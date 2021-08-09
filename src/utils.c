#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <stdbool.h>

int string_to_argv(char *argv[], int size, char *str) {
  char tok[3] = " \n";
  char *token = strtok(str, tok);
  int i = 0;

  while (token != NULL) {
    if (i == size - 1) {
      return 1;
    }

    argv[i] = token;
    token = strtok(NULL, tok);
    i++;
  }

  return i;
}

// Convert argv into a space separated string.
void concat_argv(char *dest, int in_argc, char **in_argv) {
  int i = 1;

  while (i < in_argc) {
    strcat(dest, in_argv[i]);
    if (i < in_argc) {
      strcat(dest, " ");
    }
    i++;
  }
}

// Checks if input string is a number
bool is_integer(const char *str) {
  int input_length = strlen(str);
  int temp_integer;
  char *temp_string = NULL;

  if (input_length <= 0) {
    return false;
  }

  temp_string = (char *)malloc(input_length * sizeof(char));

  // "cast" the input to temp_integer.
  sscanf(str, "%d", &temp_integer);

  // "re-cast" the temp_integer to temp_string
  //
  // sprintf will return the length of 'temp_string', so if that matches
  // the input_length we know the input is an integer.
  if (sprintf(temp_string, "%d", temp_integer) == input_length) {
    free(temp_string);
    return true;
  }
  free(temp_string);
  return false;
}
