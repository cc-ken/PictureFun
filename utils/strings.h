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

// unicode to utf8
std::string ws2s(const std::wstring& src);
// utf8 to unicode
std::wstring s2ws(const std::string& str);
//GBK 2 unicode
std::wstring  ls2ws(const std::string& input);
//unicode to GBK
std::string  ws2ls(const std::wstring& unicodeStr);
//utf8 to GBK
inline std::string s2ls(const std::string& input) {
	std::wstring unicodeStr = s2ws(input);
	return ws2ls(unicodeStr);
}

//gbk to utf8
inline std::string ls2s(const std::string& input) {
	std::wstring unicodeStr = ls2ws(input);
	return ws2s(unicodeStr);
}

}
