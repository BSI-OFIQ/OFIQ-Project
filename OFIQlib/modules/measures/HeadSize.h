/**
 * @file HeadSize.h
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
 * @brief Provides a class implementing the head size measure.
 * @author OFIQ development team
 */
#pragma once

#include "landmarks.h"
#include "Measure.h"

/**
 * @brief Provides measures implemented in OFIQ.
 */
namespace OFIQ_LIB::modules::measures
{
    /**
     * @brief Implementation of the head size measure.
     * @details Head size measure is based on the the distance T between the midpoint between the eyes and the chin 
     * and the height of the image. Check ISO/IEC 29794-5 for more information.
     */
    class HeadSize : public Measure
    {
    public:
        /**
         * @brief Constructor
         * @param configuration Configuration object from which measure-related configuration is read.
         */
        explicit HeadSize(
            const Configuration& configuration);

        /**
         * @brief Run computation of head size measure,
         * 
         * @param session Session object containing the original facial image and pre-processing results.
         */
        void Execute(OFIQ_LIB::Session & session) override;
    };
}