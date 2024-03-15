/**
 * @file Configuration.cpp
 *
 * @copyright Copyright (c) 2024  Federal Office for Information Security, Germany
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @author OFIQ development team
 */

#include "Configuration.h"

#include "OFIQError.h"
#include <filesystem>
#include <fstream>
#include <magic_enum.hpp>
#include <tao/json.hpp>

namespace fs = std::filesystem;
using jType = tao::json::type;

namespace OFIQ_LIB
{
    static void ParseObject(
        std::map<std::string, tao::json::value, std::less<>>& params,
        const tao::json::value& value,
        const std::string& path)
    {
        auto forwardPath = path;
        if (forwardPath != "")
            forwardPath += ".";
        if (value.type() == tao::json::type::OBJECT)
        {
            for (const auto& [key, val] : value.get_object())
            {
                ParseObject(params, val, forwardPath + key);
            }
        }
        else
        {
            params.try_emplace(path, value);
        }
    }

    Configuration::Configuration(const std::string& configDir, const std::string& configFilename)
    {
        fs::path configDirPath(configDir);

        m_dataDir = configDirPath;

        fs::path fullConfPath;
        fs::path pathConfFilename;
        // use default fileName 'ofiq_config.jaxn' if no configFilename was given
        pathConfFilename = configFilename.empty() ? fs::path("ofiq_config.jaxn") : fs::path(configFilename);
        
        fullConfPath = pathConfFilename.parent_path().empty() ?
            fs::weakly_canonical(configDirPath / pathConfFilename) : pathConfFilename;

        std::ifstream istream(fullConfPath.string());
        if(!istream.good())
            throw std::runtime_error("Invalid path to config file: " + fullConfPath.string());

        std::string source;
        tao::json::value jsonValue = tao::json::jaxn::from_stream(istream, source);
        ParseObject(parameters, jsonValue["config"], "");

    }

    std::string Configuration::getDataDir() const {
        return m_dataDir.string();
    }

    void Configuration::SetDataDir(std::string dataDir)
    {
        m_dataDir = dataDir;
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
        }
        return true;
    }

    bool Configuration::GetBool(const std::string& key) const
    {
        bool value;
        if (!GetBool(key, value))
            throw OFIQError(
                OFIQ::ReturnCode::MissingConfigParamError,
                "Required parameter missing: " + key);
        return value;
    }

    std::string Configuration::GetString(const std::string& key) const
    {
        std::string value;
        if (!GetString(key, value))
            throw OFIQError(
                OFIQ::ReturnCode::MissingConfigParamError,
                "Required parameter missing: " + key);
        return value;
    }

    double Configuration::GetNumber(const std::string& key) const
    {
        double value;
        if (!GetNumber(key, value))
            throw OFIQError(
                OFIQ::ReturnCode::MissingConfigParamError,
                "Required parameter missing: " + key);
        return value;
    }
}
