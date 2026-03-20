/**
 * @file InMemoryConfiguration.h
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
 * @brief Configuration backend for typed in-memory configuration.
 * @author OFIQ development team
 */

#pragma once

#include "Configuration.h"

#include <ofiq_config.h>

namespace OFIQ_LIB
{
    class InMemoryConfiguration : public Configuration
    {
    public:
        explicit InMemoryConfiguration(const OFIQ::Configuration& config);

        bool GetBool(const std::string& key, bool& value) const override;
        bool GetString(const std::string& key, std::string& value) const override;
        bool GetNumber(const std::string& key, double& value) const override;
        bool GetStringList(const std::string& key, std::vector<std::string>& value) const override;

        bool GetBool(const std::string& key) const override;
        std::string GetString(const std::string& key) const override;
        double GetNumber(const std::string& key) const override;

        std::string getDataDir() const override;
        void SetDataDir(std::string_view dataDir) override;
        std::string GetFullPath(const std::string& key) const override;

    private:
        const OFIQ::Configuration& m_config;
        mutable std::string m_dataDirOverride;
        bool m_hasDataDirOverride = false;
    };
}
