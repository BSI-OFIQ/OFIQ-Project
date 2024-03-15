/**
 * @file detectors.h
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
 * @brief Provides the interface class to the face detector implementations.
 * @author OFIQ development team
 */

#pragma once

#include "ofiq_lib.h"
#include "Session.h"

 /**
  * Namespace for OFIQ implementations.
  */
namespace OFIQ_LIB
{

    /**
     * @brief Provides the interface class to the face detector implementations.
     * @details This class provides the base class / interface for the integration of different implementations of a face detector.
     */
    class FaceDetectorInterface
    {
    public:
        /**
         * @brief Destroy the Face Detector Interface object
         * 
         */
        virtual ~FaceDetectorInterface() = default;

        /**
         * @brief This function detects faces in given image.
         *
         * @param[in] session
         * Session containing relevant information for the current task.
         */
        std::vector<OFIQ::BoundingBox> detectFaces(OFIQ_LIB::Session& session);

    protected:
        /**
         * @brief This method is to be called in derived classes to perform the detection of one/more faces on the given image.
         * 
         * @param session
         * Session containing relevant information for the current task.
         * @return std::vector<OFIQ::BoundingBox> 
         */
        virtual std::vector<OFIQ::BoundingBox> UpdateFaces(OFIQ_LIB::Session& session) = 0;
    };
}
