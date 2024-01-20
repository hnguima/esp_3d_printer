#pragma once

#include <vector>

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <esp_err.h>
#include <esp_spiffs.h>

class FileSystem
{
protected:
  static FileSystem *_singleton;
  FileSystem();

public:
  std::vector<esp_vfs_spiffs_conf_t> partitions;

  bool find(const char *file_name);
  int32_t get_size(const char *file_name);

  size_t read(const char *file_name, char *output, size_t size);
  size_t read(const char *file_name, char *output);

  size_t write(const char *file_name, char *input, size_t size);
  size_t write(const char *file_name, char *input);

  esp_err_t open(const char *file_name, const char *mode);
  esp_err_t close();
  esp_err_t abort();

  static FileSystem *get_instance();
  
  esp_err_t mount(std::string root);
  esp_err_t mount_all();

  esp_err_t unmount(std::string root);
  esp_err_t unmount_all();
  
  esp_err_t mkdir(std::string name);

  esp_err_t create(std::string name);
  esp_err_t force_create(std::string name);

  esp_err_t move(std::string name);
  esp_err_t force_move(std::string name);

  esp_err_t copy(std::string name);
  esp_err_t force_copy(std::string name);

  esp_err_t remove(std::string name);
  esp_err_t force_remove(std::string name);

  bool is_file(std::string file_name);
  bool is_dir(std::string dir_name);

  FileSystem(FileSystem &other) = delete;
  void operator=(const FileSystem &) = delete;

private:
  FILE *file;
};