#ifndef __GCODE_INTERPRETER__H
#define __GCODE_INTERPRETER__H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <sys/dirent.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include <esp_err.h>
#include <esp_system.h>
#include <esp_log.h>

// #include "filesystem.h"
// #include "sdcard.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GCODE_CMD_ZERO()        \
  {                             \
      .x = 0,                   \
      .has_x = false,           \
      .y = 0,                   \
      .has_y = false,           \
      .z = 0,                   \
      .has_z = false,           \
      .ext = 0,                 \
      .has_ext = false,         \
      .feed_rate = 0,           \
      .has_feed_rate = false,   \
      .hotend_temp = 0,         \
      .has_hotend_temp = false, \
      .hotbed_temp = 0,         \
      .has_hotbed_temp = false, \
      .wait = false,            \
  }

typedef struct
{

  float x;
  bool has_x;

  float y;
  bool has_y;

  float z;
  bool has_z;

  float ext;
  bool has_ext;

  float hotbed_temp;
  bool has_hotbed_temp;

  float hotend_temp;
  bool has_hotend_temp;

  float feed_rate;
  bool has_feed_rate;

  bool has_set_position;

  bool wait;

} gcode_cmd_t;

gcode_cmd_t gcode_interpret_line(const char *line);
int file_get_line(FILE *fp, char *line);

#ifdef __cplusplus
}
#endif 

#endif /* __GCODE_INTERPRETER__H */