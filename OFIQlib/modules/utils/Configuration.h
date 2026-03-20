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

#include <string>
#include <string_view>
#include <vector>

 /**
  * Namespace for OFIQ implementations.
  */
namespace OFIQ_LIB
{
    /**
     * @brief Abstract configuration interface used by the OFIQ implementation.
     */
    class Configuration
    {
    public:
        virtual ~Configuration() = default;

        /**
         * @brief Accesses a boolean configuration.
         * @param key Key of the configuration.
         * @param value Boolean reference to where the configuration result is stored.
         * @return <code>true</code> if the configuration was successfully accessed;
         * otherwise, if the configuration was not successfully accessed, the function
         * returns <code>false</code>.
         */
        virtual bool GetBool(const std::string& key, bool& value) const = 0;

        /**
         * @brief Accesses a string configuration.
         * @param key Key of the configuration.
         * @param value std::string reference to where the configuration result is stored.
         * @return <code>true</code> if the configuration was successfully accessed;
         * otherwise, if the configuration was not successfully accessed, the function
         * returns <code>false</code>.
         */
        virtual bool GetString(const std::string& key, std::string& value) const = 0;

        /**
         * @brief Accesses a double configuration.
         * @param key Key of the configuration.
         * @param value Reference to a double type to where the configuration result 
         * is stored.
         * @return <code>true</code> if the configuration was successfully accessed;
         * otherwise, if the configuration was not successfully accessed, the function
         * returns <code>false</code>.
         */
        virtual bool GetNumber(const std::string& key, double& value) const = 0;

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
        virtual bool GetStringList(const std::string& key, std::vector<std::string>& value) const = 0;

        /**
         * @brief Accesses a boolean configuration.
         * @param key Key of the configuration.
         * @return The accessed boolean configuration.
         * @throws OFIQ_LIB::OFIQError if the configuration was not successfully accessed.
         */
        virtual bool GetBool(const std::string& key) const = 0;

        /**
         * @brief Accesses a string configuration.
         * @param key Key of the configuration.
         * @return The accessed string configuration.
         * @throws OFIQ_LIB::OFIQError if the configuration was not successfully accessed.
         */
        virtual std::string GetString(const std::string& key) const = 0;

        /**
         * @brief Accesses a double configuration.
         * @param key Key of the configuration.
         * @return The accessed double configuration.
         * @throws OFIQ_LIB::OFIQError if the configuration was not successfully accessed.
         */
        virtual double GetNumber(const std::string& key) const = 0;

        /**
         * @brief Access configuration directory.
         * @details The configuration directory has been specified in the constructor
         * or afterwards by the \link OFIQ_LIB::Configuration::SetDataDir() SetDataDir()\endlink
         * method.
         * @return String representation of the configuration directory.
         */
        virtual std::string getDataDir() const = 0;

        /**
         * @brief Sets the configuration directory.
         * @param dataDir String representation of the configuration directory.
         * @attention The configurations will not be updated when this method is called;
         * it causes only an update of an internal private path member.
         */
        virtual void SetDataDir(std::string_view dataDir) = 0;

        /**
         * @brief Accesses a full path of a string configuration.
         * @param key Key of the configuration.
         * @return The combined path of the configuration directory and accessed string configuration.
         * @throws OFIQ_LIB::OFIQError if the configuration was not successfully accessed.
         */
        virtual std::string GetFullPath(const std::string& key) const = 0;
    };
}
