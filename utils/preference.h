#pragma once

#include <map>
#include <string>
#include <memory>

namespace pf
{

class preference {
public:
    typedef std::shared_ptr<preference> Ptr;
    virtual ~preference();

public:
    static preference& i();

    static Ptr create(const std::string& path, bool readOnly = true);

    void reload();

    void save();

    std::string& operator[](const std::string& key);

    int get(const std::string& key, int valDefault = 0);

    float get(const std::string& key, float valDefault = 0);

    std::string get(const std::string& key, std::string valDefault = "");

    void set(const std::string& key, int val);

    void set(const std::string& key, float val);

    void set(const std::string& key, const std::string& val);

protected:
    preference(const std::string& filePath = "", bool readonly = false);

    std::map<std::string, std::string> props_;
    std::string filePath_;
    bool readonly_;
};

}
