/**
 * @file JaxnConfiguration.cpp
 *
 * @copyright Copyright (c) 2026  Federal Office for Information Security, Germany
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

#include "JaxnConfiguration.h"

#include "DataStream.h"
#include "OFIQError.h"

#include <filesystem>
#include <magic_enum.hpp>
#include <tao/json.hpp>

namespace fs = std::filesystem;
using jType = tao::json::type;

namespace OFIQ_LIB
{
    struct JaxnConfiguration::Impl
    {
        std::map<std::string, tao::json::value, std::less<>> parameters;
        std::filesystem::path dataDir;
    };

    namespace
    {
        void ParseObject(
            std::map<std::string, tao::json::value, std::less<>>& params,
            const tao::json::value& value,
            const std::string& path)
        {
            std::string forwardPath = path;
            if (!forwardPath.empty())
                forwardPath += ".";

            if (value.type() == tao::json::type::OBJECT)
            {
                for (const auto& [key, val] : value.get_object())
                    ParseObject(params, val, forwardPath + key);
                return;
            }

            params.try_emplace(path, value);
        }
    }

    JaxnConfiguration::~JaxnConfiguration() = default;

    JaxnConfiguration::JaxnConfiguration(
        const std::string& configDir,
        const std::string& configFilename)
        : m_impl(std::make_unique<Impl>())
    {
        const fs::path configDirPath(configDir);
        m_impl->dataDir = configDirPath;

        const fs::path pathConfFilename =
            configFilename.empty() ? fs::path("ofiq_config.jaxn") : fs::path(configFilename);

#if defined(ANDROID)
        const fs::path fullConfPath = pathConfFilename;
#else
        const fs::path fullConfPath = pathConfFilename.parent_path().empty()
                                          ? fs::weakly_canonical(configDirPath / pathConfFilename)
                                          : pathConfFilename;
#endif

        DataStream stream(fullConfPath.string());
        if (!stream.good())
        {
            throw OFIQError(
                OFIQ::ReturnCode::MissingConfigFileError,
                "Invalid path to config file: '" + fullConfPath.string() + "'");
        }

        std::string source;
        tao::json::value jsonValue = tao::json::jaxn::from_stream(stream, source);
        ParseObject(m_impl->parameters, jsonValue["config"], "");
    }

    std::string JaxnConfiguration::getDataDir() const { return m_impl->dataDir.string(); }

    void JaxnConfiguration::SetDataDir(std::string_view dataDir) { m_impl->dataDir = dataDir; }

    bool JaxnConfiguration::GetBool(const std::string& key, bool& value) const
    {
        const auto item = m_impl->parameters.find(key);
        if (item == m_impl->parameters.cend() || !item->second.is_boolean())
            return false;
        value = item->second.get_boolean();
        return true;
    }

    bool JaxnConfiguration::GetString(const std::string& key, std::string& value) const
    {
        const auto item = m_impl->parameters.find(key);
        if (item == m_impl->parameters.cend() || !item->second.is_string())
            return false;
        value = item->second.get_string();
        return true;
    }

    bool JaxnConfiguration::GetStringList(
        const std::string& key,
        std::vector<std::string>& value) const
    {
        const auto item = m_impl->parameters.find(key);
        if (item == m_impl->parameters.cend() || !item->second.is_array())
            return false;

        for (const auto& entry : item->second.get_array())
        {
            if (entry.is_string())
                value.push_back(entry.get_string());
        }
        return true;
    }

    bool JaxnConfiguration::GetNumber(const std::string& key, double& value) const
    {
        const auto item = m_impl->parameters.find(key);
        if (item == m_impl->parameters.cend() || !item->second.is_number())
            return false;

        switch (item->second.type())
        {
        case jType::DOUBLE:
            value = item->second.get_double();
            break;
        case jType::SIGNED:
            value = static_cast<double>(item->second.get_signed());
            break;
        case jType::UNSIGNED:
            value = static_cast<double>(item->second.get_unsigned());
            break;
        default:
            throw OFIQError(
                OFIQ::ReturnCode::MissingConfigParamError,
                "invalid type in JaxnConfiguration::GetNumber: " +
                    static_cast<std::string>(magic_enum::enum_name(item->second.type())));
        }
        return true;
    }

    bool JaxnConfiguration::GetBool(const std::string& key) const
    {
        bool value = false;
        if (!GetBool(key, value))
            throw OFIQError(OFIQ::ReturnCode::MissingConfigParamError, "Required parameter missing: " + key);
        return value;
    }

    std::string JaxnConfiguration::GetString(const std::string& key) const
    {
        std::string value;
        if (!GetString(key, value))
            throw OFIQError(OFIQ::ReturnCode::MissingConfigParamError, "Required parameter missing: " + key);
        return value;
    }

    double JaxnConfiguration::GetNumber(const std::string& key) const
    {
        double value = 0.0;
        if (!GetNumber(key, value))
            throw OFIQError(OFIQ::ReturnCode::MissingConfigParamError, "Required parameter missing: " + key);
        return value;
    }

    std::string JaxnConfiguration::GetFullPath(const std::string& key) const
    {
        std::string modelPath = GetString(key);
#if defined(ANDROID)
        if (modelPath.size() >= 3 && modelPath.substr(modelPath.size() - 3) == ".gz")
            modelPath.resize(modelPath.size() - 3);
        return modelPath;
#endif
        fs::path path(modelPath);
        if (path.is_absolute())
            return path.lexically_normal().string();
        return (fs::path(getDataDir()) / path).lexically_normal().string();
    }
}
