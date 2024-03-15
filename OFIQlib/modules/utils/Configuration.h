/**
 * @file Configuration.h
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
 * @brief Provides a configuration class for handling configurations.
 * @author OFIQ development team
 */


#pragma once

#include <map>
#include <string>
#include <filesystem>

#include <tao/json/forward.hpp>
#include <tao/json/value.hpp>

 /**
  * Namespace for OFIQ implementations.
  */
namespace OFIQ_LIB
{
    /**
     * @brief Configuration class 
     * @details The class consumes the [taoJSON](https://github.com/taocpp/json)
     * library. A configuration is read from a
     * [JAXN](https://github.com/stand-art/jaxn)-formatted file.
     */
    class Configuration
    {
    public:
        /**
         * @brief Constructor. 
         * @param configDir Directory from which a JAXN configuration is read. The path
         * can be absolute or relative to the path of the current working directory.
         * @param configFilename Name of the JAXN configuration file in <code>configDir</code>.
         */
        Configuration(const std::string& configDir, const std::string& configFilename);

        /**
         * @brief Accesses a boolean configuration.
         * @param key Key of the configuration.
         * @param value Boolean reference to where the configuration result is stored.
         * @return <code>true</code> if the configuration was successfully accessed;
         * otherwise, if the configuration was not successfully accessed, the function
         * returns <code>false</code>.
         */
        bool GetBool(const std::string& key, bool& value) const;

        /**
         * @brief Accesses a string configuration.
         * @param key Key of the configuration.
         * @param value std::string reference to where the configuration result is stored.
         * @return <code>true</code> if the configuration was successfully accessed;
         * otherwise, if the configuration was not successfully accessed, the function
         * returns <code>false</code>.
         */
        bool GetString(const std::string& key, std::string& value) const;

        /**
         * @brief Accesses a double configuration.
         * @param key Key of the configuration.
         * @param value Reference to a double type to where the configuration result 
         * is stored.
         * @return <code>true</code> if the configuration was successfully accessed;
         * otherwise, if the configuration was not successfully accessed, the function
         * returns <code>false</code>.
         */
        bool GetNumber(const std::string& key, double& value) const;

        /**
         * @brief Accesses an array of strings configured.
         * @details The string list will not be emptied; any strings read will be
         * appended.
         * @param key Key of the configuration.
         * @param value Reference to a string list to where the configuration result
         * is stored.
         * @return <code>true</code> if the configuration was successfully accessed;
         * otherwise, if the configuration was not successfully accessed, the function
         * returns <code>false</code>.
         */
        bool GetStringList(const std::string& key, std::vector<std::string>& value) const;

        /**
         * @brief Accesses a boolean configuration.
         * @param key Key of the configuration.
         * @return The accessed boolean configuration.
         * @throws OFIQ_LIB::OFIQError if the configuration was not successfully accessed.
         */
        bool GetBool(const std::string& key) const;

        /**
         * @brief Accesses a string configuration.
         * @param key Key of the configuration.
         * @return The accessed string configuration.
         * @throws OFIQ_LIB::OFIQError if the configuration was not successfully accessed.
         */
        std::string GetString(const std::string& key) const;

        /**
         * @brief Accesses a double configuration.
         * @param key Key of the configuration.
         * @return The accessed double configuration.
         * @throws OFIQ_LIB::OFIQError if the configuration was not successfully accessed.
         */
        double GetNumber(const std::string& key) const;

        /**
         * @brief Access configuration directory.
         * @details The configuration directory has been specified in the constructor
         * or afterwards by the \link OFIQ_LIB::Configuration::SetDataDir() SetDataDir()\endlink
         * method.
         * @return String representation of the configuration directory.
         */
        std::string getDataDir() const;

        /**
         * @brief Sets the configuration directory.
         * @param dataDir String representation of the configuration directory.
         * @attention The configurations will not be updated when this method is called;
         * it causes only an update of an internal private path member.
         */
        void SetDataDir(std::string dataDir);

    private:
        /**
         * @brief Map holding all configuration that can be accessed using a string key. 
         */
        std::map<std::string, tao::json::value, std::less<>> parameters;

        /**
         * @brief Path to the configuration directory.
         * @details The member is set either by the constructor or by the
         * \link OFIQ_LIB::Configuration::SetDataDir() SetDataDir()\endlink method.
         */
        std::filesystem::path m_dataDir;
    };
}
