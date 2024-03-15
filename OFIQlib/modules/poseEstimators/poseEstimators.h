/**
 * @file poseEstimators.h
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
 * @brief Base class for the different implementation of pose estimation algorithms.
 * @author OFIQ development team
 */
#pragma once

#include "ofiq_lib.h"
#include "Session.h"
#include <array>

 /**
  * Namespace for OFIQ implementations.
  */
namespace OFIQ_LIB
{

    /**
     * @brief Implementation of the base class for integrating pose estimation algorithms
     *  capable of estimating three head orientation angles (yaw, pitch and roll) from a face image. 
     */
    class PoseEstimatorInterface
    {
    public:
        using EulerAngle = std::array<double, 3>;

        /**
         * @brief Standard destructor.
         * 
         */
        virtual ~PoseEstimatorInterface() = default;

        /**
         * @brief This function estimates the three head orientation angles.
         *
         * @param session Session object containing the original facial image and pre-processing results 
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing() 
         * OFIQImpl::performPreprocessing()\endlink method 
         */
        EulerAngle& estimatePose(OFIQ_LIB::Session& session);

    protected:
        /**
         * @brief Call to estimate the head orientations. Has to be implemented in the derived class.
         * 
         * @param session Containing the input image for the estimation.
         * @param pose Return the estimated pose.
         */
        virtual void updatePose(OFIQ_LIB::Session& session, EulerAngle& pose) = 0;

    private:
         /**
         * @brief id of the session that has been used in the latest request, for internal use.
         * 
         */
        std::string lastSessionId;

        /**
         * @brief Container for storing the estimated head orientations.
         * 
         */
        EulerAngle pose;
    };
}
