/**
 * @file Executor.h
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
 * @brief This class takes care of the computation of the measures activated.
 * @author OFIQ development team
 */
#pragma once

#include "Measure.h"

 /**
  * @brief Provides measures implemented in OFIQ.
  */
namespace OFIQ_LIB::modules::measures
{
    /**
     * @brief This variable enables logging to std::cout for debug purposes. By default the logging is switched off.
     */
    const static bool execLogActive = false;

    /**
     * @brief Logging function for writing debug messages to std::cout.
     * 
     * @param msg Message to be logged.
     */
    void log(const std::string_view& msg);

    /**
     * @brief This class takes care of the computation of the measures activated.
     */
    class Executor
    {
    public:
        /**
         * @brief Construct a new Executor object
         * 
         * @param measures Provide access to the activated measures.
         */
        explicit Executor(std::vector<std::unique_ptr<Measure>> measures)
            : measures{std::move(measures)}
        {
        }

        /**
         * @brief Run the computation of the activated measures on the data of the provided session.
         * 
         * @param i_currentSession Container providing the data required for the computation of the measures.
         */
        void ExecuteAll(Session & i_currentSession) const;

        /**
         * @brief Return the list of the activated measures.
         *
         */
        const std::vector<std::unique_ptr<Measure>>& GetMeasures() const { return measures; }

    private:
        /**
         * @brief Container for access to the measures to be computed.
         * 
         */
        std::vector<std::unique_ptr<Measure>> measures;
    };
}