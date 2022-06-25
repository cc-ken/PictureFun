#include "utils/i18n.h"
#include "utils/logger.h"
#include "utils/strings.h"

#include <memory>
#include <opencv2/opencv.hpp>
#include <tinyxml2.h>
#include <map>

namespace pf
{

class i18n_xml : public i18n
{
protected:
    std::string literalFilePath;
    tinyxml2::XMLDocument xmlDoc_;
    
public:
    i18n_xml() = default;
    
    void load(const std::string& filePath) override
    {
        if (xmlDoc_.LoadFile(filePath.c_str()) != tinyxml2::XML_SUCCESS) {
            LOG_ERROR("load XML file failed: " << filePath);
            return;
        }
    }

    std::string get(const std::string& dotKey, const std::string& defVal) override
    {
        auto vals = split(dotKey, ".");
        if (vals.empty()) {
            LOG_ERROR("invalid key:" << dotKey);
        }
        std::string key;
        std::string group = "general";
        if (vals.size() > 1) {
            key = vals[1];
            group = vals[0];
        } else {
            key = vals[0];
        }
        return get(group, key, defVal);
    }

    std::string get(const std::string& group, const std::string& key, const std::string& defVal) override
    {
        auto root = xmlDoc_.RootElement();
        if (!root)
            return defVal;

        auto groupNode = root->FirstChildElement(group.c_str());
        if (!groupNode) {
            LOG_WARN("xml node:" << group << " was not found in the XML file.");
            return defVal;
        }
        auto target = groupNode->FirstChildElement(key.c_str());
        if (!target) {
            LOG_WARN("xml node:" << key << " was not found in group=" << group);
            return defVal;
        }

        return target->GetText();
    }
};


i18n& i18n::i()
{
    static i18n_xml pl;

    return pl;
}

}
