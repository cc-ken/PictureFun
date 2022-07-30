#if !defined(__APPLE__)

#if defined(_WINDOWS)
#include "windows.h"
#include "ShlObj.h"
#include <objbase.h>
#include <shellapi.h>
#include <string>

namespace pf
{
    std::string getDefaultFolder() {
        char szPath[MAX_PATH] = { 0 };
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, szPath)))
        {
            return szPath;
        }

        return "";
    }

    HMODULE GetMyModuleHandle()
    {
        static int s_somevar = 0;
        MEMORY_BASIC_INFORMATION mbi;
        if (!::VirtualQuery(&s_somevar, &mbi, sizeof(mbi))) {
            return NULL;
        }
        return static_cast<HMODULE>(mbi.AllocationBase);
    }

    std::string getCurrFilePath() {
        char buffer[MAX_PATH];
        GetModuleFileNameA(GetMyModuleHandle(), buffer, MAX_PATH);
        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        return std::string(buffer).substr(0, pos);
    }

    void getScreenResolution(int& width, int& height) {
        width = (int)GetSystemMetrics(SM_CXSCREEN);
        height = (int)GetSystemMetrics(SM_CYSCREEN);
    }

    void shellExec(const std::string& docPath) {
        ShellExecuteA(NULL, "open", docPath.c_str(), NULL, NULL, 1);
    }

    bool isDark() {
        return false;
    }
}

#else

#endif

#endif
