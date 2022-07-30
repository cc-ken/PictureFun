#include "utils/strings.h"
#include <codecvt>
#if defined(_WINDOWS)
#include "windows.h"
#endif

namespace pf
{
    std::string& trim(std::string& s)
    {
        if (s.empty()) {
            return s;
        }
        
        std::string::iterator c;
        for (c = s.begin(); c != s.end() && iswspace(*c++););
        s.erase(s.begin(), --c);
        
        for (c = s.end(); c != s.begin() && iswspace(*--c););
        s.erase(++c, s.end());
        
        return s;
    }

    std::string to_lower(const std::string &in) {
        std::string s = in;
        std::for_each(s.begin(), s.end(), [](char & c) {
            c = ::tolower(c);
            });
        return s;
    }

    void split(const std::string& s, std::vector<std::string>& v, const std::string& c)
    {
        std::string::size_type pos1, pos2;
        size_t len = s.length();
        pos2 = s.find(c);
        pos1 = 0;
        while (std::string::npos != pos2)
        {
            v.emplace_back(s.substr(pos1, pos2 - pos1));

            pos1 = pos2 + c.size();
            pos2 = s.find(c, pos1);
        }
        if (pos1 != len)
            v.emplace_back(s.substr(pos1));
    }

    std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
        std::vector<std::string> results;
        split(s, results, delimiter);
        return results;
    }

    std::string& replaceAll(std::string& source, const std::string& from, const std::string& to)
    {
        std::string newString;
        newString.reserve(source.length());

        std::string::size_type lastPos = 0;
        std::string::size_type findPos;

        while (std::string::npos != (findPos = source.find(from, lastPos))) {
            newString.append(source, lastPos, findPos - lastPos);
            newString += to;
            lastPos = findPos + from.length();
        }

        newString += source.substr(lastPos);
        source.swap(newString);

        return source;
    }

    std::string current_time_string(const char* formater)
    {
        const size_t buffer_size = 1024;
        char fname[buffer_size];
        time_t t = time(0);

        struct tm* now = nullptr;
#if defined(_WINDOWS)
        struct tm nowAlloc;
        localtime_s(&nowAlloc, &t);
        now = &nowAlloc;
#else
        now = localtime(&t);
#endif

        strftime(fname, buffer_size, formater, now);

        return fname;
    }

    std::pair<std::string, std::string> splitFilePath(const std::string& filePath)
    {
        auto lnpos = filePath.rfind('\\');
        auto rnpos = filePath.rfind('/');

        if (lnpos != filePath.npos && rnpos != filePath.npos) {
            if (rnpos > lnpos) lnpos = rnpos;
        }
        else if (rnpos != filePath.npos) {
            lnpos = rnpos;
        }
        else if (rnpos == filePath.npos && lnpos == filePath.npos) {
            return std::make_pair("", filePath);
        }
        std::string basePath = filePath.substr(0, lnpos);
        std::string fileName = filePath.substr(lnpos + 1);

        return std::make_pair(basePath, fileName);
    }

    std::string ws2s(const std::wstring& src)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(src);
    }

    // utf8 to unicode
    std::wstring s2ws(const std::string& str)
    {
        std::wstring ret;
        try {
            std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
            ret = wcv.from_bytes(str);
        }
        catch (...) {
        }
        return ret;
    }

    //GBK 2 unicode
    std::wstring ls2ws(const std::string& input)
    {
        std::wstring result;

#if defined(_WINDOWS)
        int requiredUnicode = MultiByteToWideChar(CP_ACP, 0, input.data(), (int)input.length(), NULL, 0);
        wchar_t* unicodedStr = new wchar_t[requiredUnicode];
        wmemset(unicodedStr, 0, requiredUnicode);

        int parsedUnicode = MultiByteToWideChar(CP_ACP, 0, input.data(), (int)input.length(), unicodedStr, requiredUnicode);

        if (parsedUnicode == requiredUnicode) {
            result = std::wstring(unicodedStr, parsedUnicode);
        }
        delete[] unicodedStr;
#endif
        return result;
    }

    //unicode to GBK
    std::string ws2ls(const std::wstring& unicodeStr)
    {
        std::string result;

#if defined(_WINDOWS)
        int requiredAnsi = WideCharToMultiByte(CP_ACP, 0, unicodeStr.data(), (int)unicodeStr.length(), nullptr, 0, nullptr, nullptr);
        char* ansiStr = new char[requiredAnsi];
        memset(ansiStr, 0, requiredAnsi);

        int parsedAnsi = WideCharToMultiByte(CP_ACP, 0, unicodeStr.data(), (int)unicodeStr.length(), ansiStr, requiredAnsi, nullptr, nullptr);
        if (parsedAnsi == requiredAnsi) {
            result = std::string(ansiStr, requiredAnsi);
        }
        delete[] ansiStr;
#endif
        return result;
    }
}
