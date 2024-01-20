#include "gcode_interpreter.h"

const char *TAG = "gcode_runner";

int file_get_line(FILE *fp, char *line)
{
  int size = 128;
  int index = 0;
  char ch;

  do
  {
    if (index >= (size - 1))
    {
      break;
    }

    ch = fgetc(fp);
    line[index++] = ch;

  } while (!((ch == '\n') || (ch == 255)));

  line[index] = '\0';
  return index - 1;
}

int gcode_get_int(const char *str)
{
  int out = 0;
  sscanf(str, "%d", &out);
  return out;
}

float gcode_get_float(const char *str)
{
  float out = 0;
  sscanf(str, "%f", &out);
  return out;
}

gcode_cmd_t gcode_run_g0(const char *params)
{
  char *temp;
  gcode_cmd_t cmd = GCODE_CMD_ZERO();
  cmd.wait = true;

  // get the params
  if ((temp = strchr(params, 'X')) != NULL)
  {
    cmd.has_x = true;
    cmd.x = gcode_get_float(temp + 1);
  }

  if ((temp = strchr(params, 'Y')) != NULL)
  {
    cmd.has_y = true;
    cmd.y = gcode_get_float(temp + 1);
  }

  if ((temp = strchr(params, 'Z')) != NULL)
  {
    cmd.has_z = true;
    cmd.z = gcode_get_float(temp + 1);
  }

  if ((temp = strchr(params, 'E')) != NULL)
  {
    cmd.has_ext = true;
    cmd.ext = gcode_get_float(temp + 1);
  }

  if ((temp = strchr(params, 'F')) != NULL)
  {
    cmd.has_feed_rate = true;
    cmd.feed_rate = gcode_get_float(temp + 1);
  }

  return cmd;
}

gcode_cmd_t gcode_run_m109(const char *params)
{
  char *temp;
  gcode_cmd_t cmd = GCODE_CMD_ZERO();
  cmd.wait = true;

  // get the params
  if ((temp = strchr(params, 'S')) != NULL)
  {
    cmd.has_hotend_temp = true;
    cmd.hotend_temp = gcode_get_float(temp + 1);
  }

  return cmd;
}

gcode_cmd_t gcode_run_m104(const char *params)
{
  char *temp;
  gcode_cmd_t cmd = GCODE_CMD_ZERO();
  cmd.wait = false;

  // get the params
  if ((temp = strchr(params, 'S')) != NULL)
  {
    cmd.has_hotend_temp = true;
    cmd.hotend_temp = gcode_get_float(temp + 1);
  }

  return cmd;
}

gcode_cmd_t gcode_run_m190(const char *params)
{
  char *temp;
  gcode_cmd_t cmd = GCODE_CMD_ZERO();
  cmd.wait = true;

  // get the params
  if ((temp = strchr(params, 'S')) != NULL)
  {
    cmd.has_hotbed_temp = true;
    cmd.hotbed_temp = gcode_get_float(temp + 1);
  }

  return cmd;
}

gcode_cmd_t gcode_run_m140(const char *params)
{
  char *temp;
  gcode_cmd_t cmd = GCODE_CMD_ZERO();
  cmd.wait = false;

  // get the params
  if ((temp = strchr(params, 'S')) != NULL)
  {
    cmd.has_hotbed_temp = true;
    cmd.hotbed_temp = gcode_get_float(temp + 1);
  }

  return cmd;
}

gcode_cmd_t gcode_run_g92(const char *params)
{
  char *temp;
  gcode_cmd_t cmd = GCODE_CMD_ZERO();
  cmd.wait = false;

  cmd.has_set_position = true;

  // // get the params
  // if ((temp = strchr(params, 'E')) != NULL)
  // {
  //   cmd.has_e = true;
  //   cmd.hotbed_temp = gcode_get_float(temp + 1);
  // }

  return cmd;
}
gcode_cmd_t gcode_interpret_line(const char *line)
{

  gcode_cmd_t cmd = GCODE_CMD_ZERO();

  if (line[0] == ';')
  {
    return cmd;
  }

  char gcode_cmd[8];
  char params[256];
  sscanf(line, "%s%[^\n]", gcode_cmd, params);

  if ((strcmp(gcode_cmd, "G0") == 0) || (strcmp(gcode_cmd, "G1") == 0))
  {
    return gcode_run_g0(params);
  }
  else if (strcmp(gcode_cmd, "G28") == 0)
  {
    printf("go home command not implemented %s\n", gcode_cmd);
  }
  else if (strcmp(gcode_cmd, "G92") == 0)
  {
    return gcode_run_g92(params);
  }
  else if (strcmp(gcode_cmd, "M105") == 0)
  {
    printf("report temperatures not implemented %s\n", gcode_cmd);
  }
  else if (strcmp(gcode_cmd, "M104") == 0)
  {
    return gcode_run_m104(params);
  }
  else if (strcmp(gcode_cmd, "M109") == 0)
  {
    return gcode_run_m109(params);
  }
  else if (strcmp(gcode_cmd, "M140") == 0)
  {
    return gcode_run_m140(params);
  }
  else if (strcmp(gcode_cmd, "M190") == 0)
  {
    return gcode_run_m190(params);
  }
  else if (strcmp(gcode_cmd, "M82") == 0)
  {
    printf("set absolute mode not implemented %s\n", gcode_cmd);
  }
  else if (strcmp(gcode_cmd, "M107") == 0)
  {
    printf("fan off not implemented %s\n", gcode_cmd);
  }
  else
  {
    printf("command not recognized %s\n", gcode_cmd);
  }

  return cmd;
}