#pragma once

#include <vector>

#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <esp_err.h>
#include <esp_spiffs.h>

class FileSystem
{
public:

  FileSystem(std::string root);

  bool create(std::string path);

  std::string read(std::string path);
  std::string read_slice(std::string path, int size);
  uint8_t read_bytes(std::string path);

  bool write(std::string path, std::string data);
  bool write(std::string path, uint8_t data);
  bool append(std::string path, uint8_t data);

  bool rename(std::string path, std::string new_path);
  bool remove(std::string path);

private:
};
