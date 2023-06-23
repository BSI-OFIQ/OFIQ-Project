#include "Configuration.h"

#include "FaceQaError.h"
#include <filesystem>
#include <fstream>
#include <magic_enum.hpp>
#include <tao/json.hpp>

namespace fs = std::filesystem;
using jType = tao::json::type;

namespace FACEQA_LIB
{
    static void ParseObject(
        std::map<std::string, tao::json::value>& params,
        tao::json::value& value,
        const std::string& path)
    {
        auto forwardPath = path;
        if (forwardPath != "")
            forwardPath += ".";
        switch (value.type())
        {
        case tao::json::type::OBJECT:
            for (auto& pair : value.get_object())
            {
                ParseObject(params, pair.second, forwardPath + pair.first);
            }
            break;

        default:
            params.emplace(path, value);
            break;
        }
    }

    Configuration::Configuration(const std::string& configDir, const std::string& configValue)
    {
        this->configDir = configDir;
        fs::path fullConfPath, pathConfFilename, pathConfDir(configDir);
        // use default fileName 'faceqa.jaxn' if no configValue was given
        pathConfFilename = configValue.empty() ? fs::path("faceqa.jaxn") : fs::path(configValue);
        fullConfPath = fs::weakly_canonical(pathConfDir / pathConfFilename);

        std::ifstream istream(fullConfPath.string());

        std::string source;
        tao::json::value jsonValue = tao::json::jaxn::from_stream(istream, source);
        ParseObject(parameters, jsonValue, "");

        // set params that are file paths to absolute paths
        // in case of relative paths use the configDir as the base/current dir
        fs::directory_entry dataDirectory(configDir);
        for (auto& p : parameters)
        {
            if (p.first.compare(p.first.length() - 5, 5, "_path") == 0 && p.second.is_string_type())
            {
                fs::path paramFilepath(p.second.get_string());
                if (!paramFilepath.is_absolute())
                {
                    paramFilepath = fs::weakly_canonical(dataDirectory / paramFilepath);
                    p.second.set_string(paramFilepath.string());
                }
            }
        }
    }

    bool Configuration::GetBool(const std::string& key, bool& value) const
    {
        std::map<std::string, tao::json::value>::const_iterator citModel = parameters.find(key);
        if (citModel == parameters.cend())
            return false;
        if (!citModel->second.is_boolean())
            return false;
        value = citModel->second.get_boolean();
        return true;
    }

    bool Configuration::GetString(const std::string& key, std::string& value) const
    {
        std::map<std::string, tao::json::value>::const_iterator citModel = parameters.find(key);
        if (citModel == parameters.cend())
            return false;
        if (!citModel->second.is_string())
            return false;
        value = citModel->second.get_string();
        return true;
    }


    bool Configuration::GetStringList(const std::string& key, std::vector<std::string>& value) const
    {
        std::map<std::string, tao::json::value>::const_iterator citModel = parameters.find(key);
        if (citModel == parameters.cend()) {
            return false;
        }

        if (!citModel->second.is_array()) {
            return false;
        }
        
        auto theArray = citModel->second.get_array();

        for (auto entry : theArray) {
            if (entry.is_string()) {
                value.push_back(entry.get_string());
            }
        }

        return true;
    }




    bool Configuration::GetNumber(const std::string& key, double& value) const
    {
        std::map<std::string, tao::json::value>::const_iterator citModel = parameters.find(key);
        if (citModel == parameters.cend())
            return false;
        const auto& item = citModel->second;
        if (!item.is_number())
            return false;
        switch (item.type())
        {
        case jType::DOUBLE:
            value = item.get_double();
            break;
        case jType::SIGNED:
            value = static_cast<double>(item.get_signed());
            break;
        case jType::UNSIGNED:
            value = static_cast<double>(item.get_unsigned());
            break;

        default:
            throw std::runtime_error(
                "invalid type in Configuration::GetNumber: " +
                static_cast<std::string>(magic_enum::enum_name(item.type())));
            break;
        }
        return true;
    }

    bool Configuration::GetBool(const std::string& key) const
    {
        bool value;
        if (!GetBool(key, value))
            throw FaceQaError(
                FACEQA::ReturnCode::MissingConfigParamError,
                "Required parameter missing: " + key);
        return value;
    }

    std::string Configuration::GetString(const std::string& key) const
    {
        std::string value;
        if (!GetString(key, value))
            throw FaceQaError(
                FACEQA::ReturnCode::MissingConfigParamError,
                "Required parameter missing: " + key);
        return value;
    }

    double Configuration::GetNumber(const std::string& key) const
    {
        double value;
        if (!GetNumber(key, value))
            throw FaceQaError(
                FACEQA::ReturnCode::MissingConfigParamError,
                "Required parameter missing: " + key);
        return value;
    }
}
