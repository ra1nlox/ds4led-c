#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void write_colors_in(char paths[3], int colors[3]) {
  

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

  

  free(device);
  device = NULL;
  return 0;
}