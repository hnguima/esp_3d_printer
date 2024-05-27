#pragma once

#include <vector>

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <esp_err.h>
#include <esp_littlefs.h>

class Item
{
  Item(std::string path);

public:


private:

  vector<Item*> children; 

};