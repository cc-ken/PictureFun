#pragma once

#include <string>

namespace pf {

class i18n
{
public:
    virtual ~i18n() {}
    static i18n& i();

public:
    virtual std::string get(const std::string& dotKey, const std::string& defVal) = 0;
    
    virtual std::string get(const std::string& group, const std::string& key, const std::string& defVal) = 0;
    
    virtual void load(const std::string& path) = 0;
};

}

#define PF_TEXT(x, d)    pf::i18n::i().get(x, d).c_str()
