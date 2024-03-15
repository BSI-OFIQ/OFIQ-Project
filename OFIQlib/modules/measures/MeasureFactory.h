/**
 * @file MeasureFactory.h
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
 * @brief Provides a class for requesting creation of
 * measure implementations.
 * @author OFIQ development team
 */
#pragma once

#include "Configuration.h"
#include "ofiq_lib.h"
#include "Measure.h"
#include "Session.h"

 /**
  * @brief Provides measures implemented in OFIQ.
  */
namespace OFIQ_LIB::modules::measures
{
    /**
     * @brief Measure factor class 
     */
    class MeasureFactory
    {
     public:
         // Avoids instantiation from this class
         MeasureFactory() = delete;

         /**
          * @brief Requests the creation of a measure implementation.
          * @param measure Enum value encoding the requested measure.
          * @param configuration Configuration from which measure parameters
          * are read.
          * @param session Session object containing the original facial image
          * and pre-processing results
          * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
          * OFIQImpl::performPreprocessing()\endlink method.
          * @attention The function returns <code>nullptr</code> if
          * the request of a measure is not implemented by the function.
          * @attention If any constructor of a requested measures throws something,
          * the throw is forwarded to this function.
          */
         static std::unique_ptr<Measure> CreateMeasure(
            const OFIQ::QualityMeasure measure,
            const Configuration& configuration,
            OFIQ_LIB::Session& session);
    };
}
