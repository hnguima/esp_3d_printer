#include "esp_filesystem.hpp"

#include <sys/stat.h>
#include <freertos/FreeRTOS.h>

#include <esp_log.h>

#include <esp_partition.h>

static const char *TAG = "FileSystem";

FileSystem *FileSystem::_singleton = nullptr;

esp_err_t FileSystem::search_for_partitions()
{
  esp_partition_iterator_t partition_iterator = esp_partition_find(
      ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, NULL);

  if (partition_iterator == NULL)
  {
    ESP_LOGE(TAG, "Couldn't find any FS partition");
    return ESP_ERR_NOT_FOUND;
  }

  const esp_partition_t *partition = NULL;

  do
  {
    partition = esp_partition_get(partition_iterator);

    ESP_LOGI(TAG, "Found partition: %s", partition->label);

    char *base_path = (char *)malloc(sizeof(char) * 32);
    base_path[0] = '/';
    strcpy(base_path + 1, partition->label);

    esp_vfs_littlefs_conf_t littlefs_config = {
        .base_path = base_path,
        .partition_label = partition->label,
        // .max_files = 1,
        .format_if_mount_failed = false,
        .dont_mount = false,
    };

    FileSystem::_singleton->partitions.push_back(littlefs_config);

    partition_iterator = esp_partition_next(partition_iterator);
  } while (partition_iterator != NULL);

  return ESP_OK;
}

FileSystem::FileSystem()
{
  // FileSystem::_singleton->file = nullptr;
  // ESP_LOGI(TAG, "chegou");
}

FileSystem *FileSystem::get_instance()
{

  if (FileSystem::_singleton == nullptr)
  {
    FileSystem::_singleton = new FileSystem();
    FileSystem::_singleton->search_for_partitions();
  }
  return FileSystem::_singleton;
}

esp_err_t FileSystem::mount(std::string root)
{
  esp_err_t err = ESP_OK;

  if (FileSystem::_singleton == nullptr)
  {
    FileSystem::get_instance();
  }

  if (FileSystem::_singleton->partitions.empty())
  {
    ESP_LOGE(TAG, "Partition amount is invalid: %d", FileSystem::_singleton->partitions.size());
    return ESP_ERR_INVALID_SIZE;
  }

  for (esp_vfs_littlefs_conf_t config : FileSystem::_singleton->partitions)
  {

    if (std::string(config.base_path) == root)
    {
      err = esp_vfs_littlefs_register(&config);

      if (err == ESP_FAIL)
      {
        ESP_LOGE(TAG, "Failed to mount or format filesystem");
        return err;
      }
      if (err == ESP_ERR_NOT_FOUND)
      {
        ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        return err;
      }
      if (err != ESP_OK)
      {
        ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(err));
        return err;
      }

      ESP_LOGI(TAG, "Mounted: %s", config.base_path);
      return err;
    }
  }

  ESP_LOGE(TAG, "Failed to find SPIFFS partition (%s)", root.c_str());
  return ESP_ERR_NOT_FOUND;
}

esp_err_t FileSystem::mount_all()
{
  esp_err_t err = ESP_OK;

  if (FileSystem::_singleton == nullptr)
  {
    FileSystem::get_instance();
  }

  if (FileSystem::_singleton->partitions.empty())
  {
    ESP_LOGE(TAG, "Partition amount is invalid: %d", FileSystem::_singleton->partitions.size());
    return ESP_ERR_INVALID_SIZE;
  }

  for (esp_vfs_littlefs_conf_t config : FileSystem::_singleton->partitions)
  {
    err = esp_vfs_littlefs_register(&config);

    if (err == ESP_FAIL)
    {
      ESP_LOGE(TAG, "Failed to mount or format filesystem");
      return err;
    }
    if (err == ESP_ERR_NOT_FOUND)
    {
      ESP_LOGE(TAG, "Failed to find SPIFFS partition");
      return err;
    }
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "Failed to initialize SPIFFS (%d)", err);
      return err;
    }

    ESP_LOGI(TAG, "Mounted: %s", config.base_path);
  }

  return err;
}

esp_err_t FileSystem::unmount(std::string root)
{
  esp_err_t err = ESP_OK;

  if (FileSystem::_singleton == nullptr)
  {
    FileSystem::get_instance();
  }

  for (esp_vfs_littlefs_conf_t config : FileSystem::_singleton->partitions)
  {
    if (std::string(config.base_path) == root)
    {
      err = esp_vfs_littlefs_unregister(config.partition_label);
      if (err != ESP_OK)
      {
        ESP_LOGE(TAG, "Failed unmounting: %s", config.partition_label);
        return err;
      }

      ESP_LOGI(TAG, "Unmounted: %s", config.partition_label);
      return err;
    }
  }

  ESP_LOGE(TAG, "Failed to find SPIFFS partition (%d)", err);
  return ESP_ERR_NOT_FOUND;
}

esp_err_t FileSystem::unmount_all()
{
  esp_err_t err = ESP_OK;

  if (FileSystem::_singleton == nullptr)
  {
    FileSystem::get_instance();
  }

  for (esp_vfs_littlefs_conf_t config : FileSystem::_singleton->partitions)
  {
    err = esp_vfs_littlefs_unregister(config.partition_label);
    ESP_LOGI(TAG, "Unmounted: %s", config.partition_label);
  }

  return err;
}

esp_err_t FileSystem::make_dir(std::string name)
{
  const char *dir_name = name.c_str();

  if (mkdir(dir_name, S_IRWXU | S_IRWXG | S_IRWXO) == 0)
  {
    return ESP_OK;
  }

  perror("mkdir failed");
  return ESP_FAIL;
}

esp_err_t FileSystem::create(std::string name)
{
  FILE *f = fopen(name.c_str(), "w");
  if (f == NULL)
  {
    ESP_LOGE(TAG, "Failed to open file for writing");
    return ESP_FAIL;
  }

  fclose(f);

  return ESP_OK;
}
esp_err_t FileSystem::force_create(std::string name)
{
  return ESP_FAIL;
}

esp_err_t FileSystem::move(std::string src, std::string dest)
{
  return ESP_FAIL;
}
esp_err_t FileSystem::force_move(std::string src, std::string dest)
{
  return ESP_FAIL;
}

esp_err_t FileSystem::copy(std::string src, std::string dest)
{
  return ESP_FAIL;
}

esp_err_t FileSystem::force_copy(std::string src, std::string dest)
{
  return ESP_FAIL;
}

esp_err_t FileSystem::remove(std::string name)
{
  if (unlink(name.c_str()) == 0)
  {
    return ESP_OK;
  }

  perror("unlink failed");
  return ESP_FAIL;
}
esp_err_t FileSystem::force_remove(std::string name)
{
  return ESP_FAIL;
}

bool FileSystem::is_file(std::string file_name)
{
  struct stat statbuf;
  if (stat(file_name.c_str(), &statbuf) == 0)
  {
    return S_ISREG(statbuf.st_mode);
  }

  perror("is file");
  return false;
}
bool FileSystem::is_dir(std::string dir_name)
{
  struct stat statbuf;
  if (stat(dir_name.c_str(), &statbuf) == 0)
  {
    return S_ISDIR(statbuf.st_mode);
  }

  perror("is dir");
  return false;
}

// esp_err_t FileSystem::open(const char *file_name, const char *mode)
// {
//   // TODO: mutex take here

//   if (FileSystem::_singleton->file)
//   {
//     ESP_LOGE(TAG, "A file is already open, close it before reading another one");
//     return ESP_ERR_INVALID_STATE;
//   }

//   this->file = fopen(file_name, mode);

//   return ESP_OK;
// }

// esp_err_t FileSystem::close()
// {
//   if (this->file == NULL)
//   {
//     ESP_LOGE(TAG, "File stream not open");
//     return ESP_FAIL;
//   }

//   fclose(this->file);
//   this->file = NULL;

//   // TODO: mutex give here

//   return ESP_OK;
// }

// esp_err_t FileSystem::abort()
// {
//   return this->close();
// }

// size_t FileSystem::read(const char *file_name, char *output)
// {
//   return this->read(file_name, output, this->get_size(file_name));
// }

// size_t FileSystem::read(const char *file_name, char *output, size_t size)
// {

//   if (this->file == NULL)
//   {
//     ESP_LOGI(TAG, "Opening file: %s (r)", file_name);

//     if (!this->find(file_name))
//     {
//       return 0;
//     }

//     this->open(file_name, "rb");
//   }

//   // Read 'size' bytes:
//   size_t read_size = fread(output, 1, size, this->file);

//   if (feof(this->file))
//   {
//     fclose(this->file);
//     this->file = NULL;
//   }

//   return read_size;
// }

// size_t FileSystem::write(const char *file_name, char *input)
// {
//   return this->write(file_name, input, strlen(input));
// }

// size_t FileSystem::write(const char *file_name, char *input, size_t size)
// {

//   if (this->file == NULL)
//   {
//     ESP_LOGI(TAG, "Opening file: %s (w)", file_name);
//     this->open(file_name, "wb");
//   }

//   // write 'size' bytes:
//   size_t written_size = fwrite(input, 1, size, this->file);

//   return written_size;
// }

// bool FileSystem::find(const char *file_name)
// {
//   struct stat st;
//   if (stat(file_name, &st) != 0)
//   {
//     ESP_LOGE(TAG, "File not found: %s", file_name);
//     perror("stat");
//     return false;
//   }

//   return true;
// }

// int32_t FileSystem::get_size(const char *file_name)
// {

//   struct stat st;
//   if (stat(file_name, &st) != 0)
//   {
//     ESP_LOGE(TAG, "File not found: %s", file_name);
//     perror("stat");
//     return false;
//   }

//   int32_t size = st.st_size;

//   return size;
// }
