#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NOB_IMPLEMENTATION
#include "nob.h"

#define RED_PATH "/sys/class/leds/%s:red/brightness"
#define GREEN_PATH "/sys/class/leds/%s:green/brightness"
#define BLUE_PATH "/sys/class/leds/%s:blue/brightness"

#define BUFFER_SIZE 64

char* get_device_name() {
  char* buffer = malloc(BUFFER_SIZE);

  if (buffer == NULL) {
    return NULL;
  }

  FILE *fp = popen("udevadm info -a -p $(udevadm info -q path -n /dev/input/js0 2>&1) 2>&1 | grep -oE 'input[0-9]+' 2>&1 | head -n1 | tr -d '\n'", "r");

  if (fp == NULL) {
      perror("popen failed");
      free(buffer);
      buffer = NULL;
      return NULL;
  }

  if (fgets(buffer, BUFFER_SIZE, fp) == NULL) {
    pclose(fp);
    free(buffer);
    buffer = NULL;
    return NULL;
  }

  int status = pclose(fp);

  if (status == -1) {
    perror("pclose failed");
    free(buffer);
    buffer = NULL;
    return NULL;
  }

  return buffer;
}

void write_colors_in(char *paths[3], int colors[3]) {
  for (int i = 0; i < 3; ++i) {
    FILE *fptr = fopen(paths[i], "w");
    
    if (fptr == NULL) {
      nob_log(NOB_ERROR, "Error opening file");
      return;
    }
    
    fprintf(fptr, "%d", colors[i]);

    fclose(fptr);
  }
  return;
}

int* hex_to_dec(char* hex) {
  if (strlen(hex) != 6) {
    nob_log(NOB_ERROR, "Wrong hex number (e.g 000040)");
    return NULL;
  }
  int *color_values = malloc(sizeof(int) * 3);
  int counter = 0;
  for (int i = 0; i < 6; i+=2) {
    char pair[2] = {hex[i], hex[i+1]};
    
    int decimalnumber, i;

    int cnt;

    int digit;

    cnt = 0;
    decimalnumber = 0;
  
    for (i = (strlen(pair) - 1); i >= 0; i--) {
        switch (pair[i]) {
        case 'A':
            digit = 10;
            break;
        case 'a':
            digit = 10;
            break;
        case 'B':
            digit = 11;
            break;
        case 'b':
            digit = 11;
            break;
        case 'C':
            digit = 12;
            break;
        case 'c':
            digit = 12;
            break;
        case 'D':
            digit = 13;
            break;
        case 'd':
            digit = 13;
            break;
        case 'E':
            digit = 14;
            break;
        case 'e':
            digit = 14;
            break;
        case 'F':
            digit = 15;
            break;
        case 'f':
            digit = 15;
            break;
        default:
            digit = pair[i] - 0x30;
        }

        decimalnumber = decimalnumber + (digit)*pow((double)16, (double)cnt);
        cnt++;
    }
    color_values[counter] = decimalnumber;
    ++counter;
  }
  return color_values;
}


int main(int argc, char **argv) {
  char* device = get_device_name();
  
  if (device == NULL) {
    nob_log(NOB_ERROR, "The gamepad is not connected");
    return 1;
  }
  
  if (device[0] == '\0') {
    free(device);
    device = NULL;
    nob_log(NOB_ERROR, "The gamepad is not connected");
    return 1;
  };

  char red_path_fmt[BUFFER_SIZE];
  char green_path_fmt[BUFFER_SIZE];
  char blue_path_fmt[BUFFER_SIZE];

  snprintf(red_path_fmt, sizeof(red_path_fmt), RED_PATH, device);
  snprintf(green_path_fmt, sizeof(red_path_fmt), GREEN_PATH, device);
  snprintf(blue_path_fmt, sizeof(red_path_fmt), BLUE_PATH, device);

  char* led_paths[] = {red_path_fmt, green_path_fmt, blue_path_fmt};

  // shift args to remove unnecessary filename as first argument
  nob_shift(argv, argc);

  // for (int i = 0; i < argc; ++i) {
  //   printf("argc: %d | argv: %s\n", i, argv[i]);
  // }

  switch (argc) {
    case 1:
      int* color_values = hex_to_dec(argv[0]);
      write_colors_in(led_paths, color_values);
      free(color_values);
      color_values = NULL;
      break;
    case 3:
      int colors[3] = {atoi(argv[0]), atoi(argv[1]), atoi(argv[2])};
      write_colors_in(led_paths, colors);
      break;
    default:
      nob_log(NOB_ERROR, "Wrong arguments. Must be either hex or R G B values");
      return 1;
  }

  free(device);
  device = NULL;
  return 0;
}