#pragma once

#include <esp_err.h>
#include "esp_settings.hpp"
#include "main.pb.h"

esp_err_t init_app_info_updater(Settings<MainConfig> *settings);
// esp_err_t init_settings_http_handler();
