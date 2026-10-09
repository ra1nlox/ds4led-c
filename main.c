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

#define CONFIG_PATH "/.config/ds4led/ds4led.json"

typedef struct {
  char *key;
  char *value;
} Pair;

typedef struct {
  char **items;
  size_t count;
  size_t capacity;
} String_List;

typedef struct {
  int *items;
  size_t count;
  size_t capacity;
} Int_List;

typedef struct {
  Pair *items;
  size_t count;
  size_t capacity;
} Dict;

Dict serialize_config_file(char* config_file_content) {
  Dict dict = {0};

  Int_List positions = {0};
  String_List keys = {0};
  String_List values = {0};

  for (size_t i = 0; i < strlen(config_file_content); ++i) {
    char tok = config_file_content[i];

    switch (tok) {
      case '\"':
        da_append(&positions, i);
        break;
      default:
        break;
      }
  }

  if (positions.count % 2 != 0) {
    nob_log(NOB_ERROR, "Wrong config format. Not valid JSON");
    return dict;
  }

  for (size_t i = 0; i < positions.count; i+=4) {
    String_Builder str = {0};
    for (size_t s = positions.items[i]+1; s < (size_t) positions.items[i+1]; ++s) {
      da_append(&str, config_file_content[s]);
    }
    da_append(&str, '\0');
    da_append(&keys, str.items);
  }

  for (size_t i = 2; i < positions.count; i+=4) {
    String_Builder str = {0};
    for (size_t s = positions.items[i]+2; s < (size_t) positions.items[i+1]; ++s) {
      da_append(&str, config_file_content[s]);
    }
    da_append(&str, '\0');
    da_append(&values, str.items);
  }

  for (size_t i = 0; i < keys.count; ++i) {
    Pair pair = {keys.items[i], values.items[i]};
    da_append(&dict, pair);
  }
  

  return dict;
}

bool check_config_file_existance() {
  char *home_dir = getenv("HOME");
  
  if (home_dir == NULL) {
    nob_log(NOB_ERROR, "Error getting user's home dir");
    return NULL;
  }

  char path[PATH_MAX];
  snprintf(path, sizeof(path), "%s%s", home_dir, CONFIG_PATH);

  FILE *fptr = fopen(path, "rb");
  
  if (fptr == NULL) {
    return false;
  } else {
    fclose(fptr);
    return true;
  }
}

int create_config_file() {
  char *home_dir = getenv("HOME");
  
  if (home_dir == NULL) {
    nob_log(NOB_ERROR, "Error getting user's home dir");
    return 1;
  }

  char path[PATH_MAX];
  snprintf(path, sizeof(path), "%s%s", home_dir, CONFIG_PATH);

  FILE *fptr = fopen(path, "w");

  if (fptr == NULL) {
    nob_log(NOB_ERROR, "Error creating config file");
  }

  const char *default_config_content = "{\n    \"default\":\"#000040\",\n}";

  fprintf(fptr, "%s", default_config_content);

  nob_log(NOB_INFO, "Created config file");
  fclose(fptr);
  return 0;
}

char *read_config_file() {
  char *home_dir = getenv("HOME");
  
  if (home_dir == NULL) {
    nob_log(NOB_ERROR, "Error getting user's home dir");
    return NULL;
  }

  char path[PATH_MAX];
  snprintf(path, sizeof(path), "%s%s", home_dir, CONFIG_PATH);

  FILE *fptr = fopen(path, "rb");

  fseek(fptr, 0, SEEK_END);
  long file_size = ftell(fptr);
  rewind(fptr);

  char *config_file_content = malloc(file_size + 1);

  if (config_file_content == NULL) {
      fclose(fptr);
      return NULL;
  }

  size_t bytes_read = fread(
      config_file_content,
      1,
      file_size,
      fptr
  );

  fclose(fptr);

  if (bytes_read != (size_t)file_size) {
      free(config_file_content);
      return NULL;
  }

  config_file_content[bytes_read] = '\0';

  return config_file_content;
}

char *get_device_name() {
  char *buffer = malloc(BUFFER_SIZE);

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

int *hex_to_dec(char* hex) {
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
  // shift args to remove unnecessary filename as first argument
  nob_shift(argv, argc);

  if (!check_config_file_existance()) {
    create_config_file();
    return 0;
  }
  char *config = read_config_file();

  if (config == NULL) {
    return 0;
  }

  Dict serial = serialize_config_file(config);
  
  if (argc == 1) {
    if (strcmp(argv[0], "-l") == 0) {
      for (size_t i = 0; i < serial.count; ++i) {
        printf("%-10s: %s\n", serial.items[i].key, serial.items[i].value);
      }
    }
      da_free(serial);
    free(config);
    config = NULL;
    return 0;
  }
  
  char *device = get_device_name();
  
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

  switch (argc) {
    case 1:
      if (strcmp(argv[0], "-l") != 0) {
        int *color_values = hex_to_dec(argv[0]);
        write_colors_in(led_paths, color_values);
        free(color_values);
        color_values = NULL;
      }
      break;
    case 2:
      if (strcmp(argv[0], "-p") == 0) {
        for (size_t i = 0; i < serial.count; ++i) {
          if (strcmp(serial.items[i].key, argv[1]) == 0) {
            int *color_values = hex_to_dec(serial.items[i].value);
            write_colors_in(led_paths, color_values);
            free(color_values);
            color_values = NULL;
          }
        }
      }
      break;
    case 3:
      int colors[3] = {atoi(argv[0]), atoi(argv[1]), atoi(argv[2])};
      write_colors_in(led_paths, colors);
      break;
    default:
      nob_log(NOB_ERROR, "Wrong arguments. Must be either hex or R G B values");
      return 1;
  }

  da_free(serial);
  free(config);
  free(device);
  config = NULL;
  device = NULL;
  return 0;
}