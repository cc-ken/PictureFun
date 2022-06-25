#include "utils/preference.h"
#include "utils/system.h"
#include "utils/logger.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utils/filesystem.hpp>

using namespace pf;

preference::preference(const std::string& filePath, bool readonly)
    : filePath_(filePath)
    , readonly_(readonly)
{
    if (filePath_.empty())
        filePath_ = pf::getDefaultFolder() + "/pf/preference.yml";
    reload();
}

preference::~preference() {
    if (!readonly_)
        save();
}

void preference::reload() {
    props_.clear();
    try {
        cv::FileStorage fs(filePath_, cv::FileStorage::READ);
        LOG_INFO("reload user preference file: [" << filePath_ << "]");
        if (fs.isOpened()) {
            auto rootNode = fs.root();
            for (const auto &node : rootNode) {
                std::string key = node.name();
                std::string val = node;

                LOG_INFO("user preference, key=" << key << ", val=" << val);
                props_[key] = val;
            }
        }
    }
    catch (...) {
        if (cv::utils::fs::exists(filePath_)) {
            cv::FileStorage fs(filePath_, cv::FileStorage::WRITE);
            LOG_WARN("delete broken user preference file");
        }
    }
}

void preference::save() {
    cv::FileStorage fs(filePath_, cv::FileStorage::WRITE);
    for (auto it : props_) {
        fs << it.first << it.second;
    }
    fs.release();
}

std::string& preference::operator[](const std::string& key)
{
    return props_[key];
}

float preference::get(const std::string& key, float valDefault)
{
    try {
        auto it = props_.find(key);
        if (it == props_.end())
            return valDefault;

        return std::stof(it->second);
    }
    catch (...) {
    }

    return valDefault;
}

int preference::get(const std::string& key, int valDefault)
{
    try {
        auto it = props_.find(key);
        if (it == props_.end())
            return valDefault;

        return std::stoi(it->second);
    }
    catch (...) {
    }

    return valDefault;
}

std::string preference::get(const std::string& key, std::string valDefault)
{
    auto it = props_.find(key);
    if (it == props_.end())
        return valDefault;

    return it->second;
}

void preference::set(const std::string& key, int val) {
    props_[key] = std::to_string(val);
}

void preference::set(const std::string& key, float val) {
    props_[key] = std::to_string(val);
}

void preference::set(const std::string& key, const std::string& val) {
    props_[key] = val;
}

preference& preference::i() {
    static preference pref;

    return pref;
}

preference::Ptr preference::create(const std::string& path, bool readOnly)
{
    preference::Ptr pf(new preference(path, readOnly));

    return pf;
}
