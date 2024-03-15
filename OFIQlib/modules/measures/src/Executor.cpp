/**
 * @file Executor.cpp
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

#include "Executor.h"

namespace OFIQ_LIB::modules::measures
{

    void log(const std::string_view& msg)
    {
        if (execLogActive)
            std::cout << msg;
    }

    void Executor::ExecuteAll(Session & i_currentSession) const
    {
        int i = 1;
        log("\t");
        for (const auto& measure : measures)
        {
            log(std::to_string(i++) + ". " + measure->GetName() + " ");
            try {
                measure->Execute(i_currentSession);
            }
            catch (...)
            {
                measure->SetQualityMeasure(i_currentSession, measure->GetQualityMeasure(), .0f, OFIQ::QualityMeasureReturnCode::FailureToAssess);
                log("Exception in " + measure->GetName() + "!!! ");
            }
        }
        log("\nfinished\n");
    }
}