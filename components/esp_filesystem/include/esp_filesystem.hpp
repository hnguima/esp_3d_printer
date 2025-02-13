#pragma once

#include <vector>

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <esp_err.h>
#include <esp_littlefs.h>

#include <Item.hpp>
#include <Folder.hpp>
#include <File.hpp>

class FileSystem
{
protected:
  FileSystem();
  static FileSystem *_singleton;

public:
  std::vector<esp_vfs_littlefs_conf_t>
      partitions;

  bool find(const char *file_name);
  int32_t get_size(const char *file_name);

  size_t read(const char *file_name, char *output, size_t size);
  size_t read(const char *file_name, char *output);

  size_t write(const char *file_name, char *input, size_t size);
  size_t write(const char *file_name, char *input);

  static esp_err_t open(const char *file_name, const char *mode);
  static esp_err_t close();
  static esp_err_t abort();

  static FileSystem *get_instance();

  static esp_err_t mount(std::string root);
  static esp_err_t mount_all();

  static esp_err_t unmount(std::string root);
  static esp_err_t unmount_all();

  static esp_err_t make_dir(std::string name);

  static esp_err_t create(std::string name);
  static esp_err_t force_create(std::string name);

  static esp_err_t move(std::string src, std::string dest);
  static esp_err_t force_move(std::string src, std::string dest);

  static esp_err_t copy(std::string src, std::string dest);
  static esp_err_t force_copy(std::string src, std::string dest);

  static esp_err_t remove(std::string name);
  static esp_err_t force_remove(std::string name);

  static bool is_file(std::string file_name);
  static bool is_dir(std::string dir_name);

  FileSystem(FileSystem &other) = delete;
  void operator=(const FileSystem &) = delete;

private:
  Item *root;
  Folder *pwd;

  esp_err_t search_for_partitions();
};