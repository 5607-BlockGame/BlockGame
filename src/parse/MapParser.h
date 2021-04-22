#pragma once


#include <string>
#include <repr/Map.h>
#include <fstream>

namespace MapParser {
    Map parseMap(const std::string &name);
};

