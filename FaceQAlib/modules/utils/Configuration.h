#pragma once

#include <map>
#include <string>
#include <tao/json/forward.hpp>
#include <tao/json/value.hpp>

namespace FACEQA_LIB
{
    class Configuration
    {
    public:
        Configuration(const std::string& configDir, const std::string& configValue);

        bool GetBool(const std::string& key, bool& value) const;
        bool GetString(const std::string& key, std::string& value) const;
        bool GetNumber(const std::string& key, double& value) const;
        bool GetStringList(const std::string& key, std::vector<std::string>& value) const;

        bool GetBool(const std::string& key) const;
        std::string GetString(const std::string& key) const;
        double GetNumber(const std::string& key) const;

    private:
        std::map<std::string, tao::json::value> parameters;

        std::string configDir;

    public:
        struct Tags
        {
            static inline const std::string generateDebugImages = "generateDebugImages";
        };
    };
}
