#pragma once

#include <string>
#include <sstream>

namespace pf
{
    std::string getDefaultFolder();

	std::string getCurrFilePath();

	void getScreenResolution(int &width, int &height);

	void shellExec(const std::string& docPath);
}
