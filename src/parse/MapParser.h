#pragma once


#include <string>
#include <repr/World.h>
#include <fstream>

namespace MapParser {
    Map parseMap(const std::string &name);
};

