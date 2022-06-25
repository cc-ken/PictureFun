#pragma once

#include <string>
#include <fstream>
#include <map>
#include <vector>
#include "time.h"
#include <algorithm>
#include <chrono>

namespace pf {

std::string& trim(std::string& s);

std::string to_lower(const std::string &in);

std::vector<std::string> split(const std::string& s, const std::string& delimiter);

std::string& replaceAll(std::string& source, const std::string& from, const std::string& to);

std::string current_time_string(const char* formater = "%y%m%d%H%M%S");

std::pair<std::string, std::string> splitFilePath(const std::string& filePath);

}
