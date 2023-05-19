#pragma once

#include <iostream>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <esp_err.h>
#include <esp_spiffs.h>

#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>

#include <esp_log.h>
#include <esp_filesystem.hpp>

template <typename T>
class Settings
{
protected:
public:
  Settings(const char *file_name)
  {
    this->obj = (T *)malloc(sizeof(T));
    strcpy(this->file_name, file_name);

    strcpy(this->default_file_name, file_name);
    sprintf(strchr(this->default_file_name, '.'), ".def");

    strcpy(this->backup_file_name, file_name);
    sprintf(strchr(this->backup_file_name, '.'), ".bak");

    // this->default_file_name = 
    //     this->backup_file_name =
            // char *ext_pointer = strchr(file_name, '.');

    // this->file_name = file_name;

    // printf(sprintf(ext_pointer, "def"));

    // this->file_name = file_name;

    FileSystem *fs = FileSystem::get_instance();

    uint8_t *buffer;
    size_t buffer_size;

    if (fs->find(file_name))
    {
      buffer_size = fs->get_size(file_name);
      buffer = (uint8_t *)malloc(sizeof(uint8_t) * buffer_size);

      fs->read(file_name, (char *)buffer, buffer_size);
      fs->close();

      // ESP_LOGI(TAG, "size: %d", buffer_size);
      // ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, buffer_size, ESP_LOG_INFO);

      this->decode((uint8_t *)buffer, buffer_size);
    }
    else
    {
      this->decode(NULL, 0);
      this->encode(&buffer, &buffer_size);

      // ESP_LOGI(TAG, "size: %d", buffer_size);
      // ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, buffer_size, ESP_LOG_INFO);

      fs->write(file_name, (char *)buffer, buffer_size);
      fs->close();

      fs->write(this->default_file_name, (char *)buffer, buffer_size);
      fs->close();
    }

    // xTaskCreate(Settings::task, "settings_backup", 4096, (void *)this, 12, NULL);

    // printf("baud is %lu!\n", this->obj->channel_1.rtu_baud_rate);
    // printf("ssid is %s!\n", this->obj->wifi.ssid);
    // printf("pass is %s!\n", this->obj->wifi.password);

    // // TODO: if not found search in FS for bak\filename.set
    // // TODO: if not found search in FS for filename.def
    // // TODO: if not found search in FS for bak\filename.def
    // // TODO: if not found abort and return possibly fs corruption
    // // TODO: if not found generate file if force tag
  };

  bool decode(uint8_t *buffer, size_t buffer_length)
  {
    bool status;

    nanopb::MessageDescriptor<T> descriptor;
    pb_istream_t stream = pb_istream_from_buffer((const unsigned char *)buffer, buffer_length);

    status = pb_decode_ex(&stream, descriptor.fields(), this->obj, PB_DECODE_NULLTERMINATED);

    if (!status)
    {
      printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
    }

    return 0;
  };

  bool encode(uint8_t **message, size_t *message_length)
  {
    bool status;

    nanopb::MessageDescriptor<T> descriptor;

    pb_get_encoded_size((unsigned int *)message_length, descriptor.fields(), this->obj);
    *message = (uint8_t *)malloc(sizeof(uint8_t) * (*message_length));

    pb_ostream_t stream = pb_ostream_from_buffer(*message, (*message_length));
    status = pb_encode(&stream, descriptor.fields(), this->obj);

    if (!status)
    {
      printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
    }

    return 0;
  };

  bool save()
  {
    return this->save(this->file_name);
  };

  bool save(const char *file_name)
  {
    uint8_t *buffer;
    size_t buffer_size;

    FileSystem *fs = FileSystem::get_instance();

    this->encode(&buffer, &buffer_size);

    fs->write(file_name, (char *)buffer, buffer_size);
    fs->close();

    return true;
  };

  T *obj;

private:
  char file_name[64];
  char default_file_name[64];
  char backup_file_name[64];

  static void task(void *param)
  {
    Settings *settings = (Settings *)param;
  };
};
