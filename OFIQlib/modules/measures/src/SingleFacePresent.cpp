/**
 * @file SingleFacePresent.cpp
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

#include "SingleFacePresent.h"
#include "landmarks.h"
#include "utils.h"
#include "OFIQError.h"

namespace OFIQ_LIB::modules::measures
{
    static const auto qualityMeasure = OFIQ::QualityMeasure::SingleFacePresent;

    SingleFacePresent::SingleFacePresent(
        const Configuration& configuration, Session& session)
        : Measure{ configuration, session, qualityMeasure }
    {
        // Note: Mapping the native score f to the quality measure qc=100*(1-f)
        // seems not to be possing using SigmoidParameter; thus this is set
        // customized within the SingleFacePresent::Execute method.
    }

    void SingleFacePresent::Execute(OFIQ_LIB::Session & session)
    {
#ifdef OFIQ_SINGLE_FACE_PRESENT_WITH_TMETRIC
        float f = 1.0f;
        auto detectedLandmarksList = session.getLandmarksAllFaces();

        if (detectedLandmarksList.size() == 0)
        {
            if (session.getDetectedFaces().size() == 1)
            {
                f = 0.0f;
            }
        }
        else if (detectedLandmarksList.size() == 1)
        {
            f = 0.0f;
        }
        else if (detectedLandmarksList.size() > 1)
        {
            // Determine the two largest T-metrics over detected faces
            float T1 = 0.0f, T2 = 0.0f;
            for (auto& lms : detectedLandmarksList)
            {
                float T = tmetric(lms);
                if (T > T1)
                {
                    T2 = T1;
                    T1 = T;
                }
                else if (T > T2)
                {
                    T2 = T;
                }
            }
            // Now 'T1' will be the largest T-metric and 'T2' will be 
            // the second largest T-metric.

            // Update face unicity: the more it approaches zero, the more
            // dominant is the largest face.
            f = sqrtf(T2 / T1);
        }

        float qc = round(100.0f * (1.0f - f));
        session.assessment().qAssessments[qualityMeasure] = { static_cast<double>(f),static_cast<double>(qc), OFIQ::QualityMeasureReturnCode::Success };
#else
        float f = 1.0f;
        auto& detectedFaces = session.getDetectedFaces();
        if (detectedFaces.size() == 1)
        {
            f = 0.0f;
        }
        else if (detectedFaces.size() > 1)
        {
            // Determine the two largest face bounding box areas
            float a1 = 0.0f, a2 = 0.0f;
            for (auto& face : detectedFaces)
            {
                float a = (float)face.width * (float)face.height;
                if (a > a1)
                {
                    a2 = a1;
                    a1 = a;
                }
                else if (a > a2)
                {
                    a2 = a;
                }
            }
            // Now 'a1' will be the largest area and 'a2' will be 
            // the second largest area.

            // Update face unicity: the more it approaches zero, the more
            // dominant is the largest face.
            f = sqrtf(a2 / a1);
        }

        float qc = round(100.0f * (1.0f - f));
        session.assessment().qAssessments[qualityMeasure] = { static_cast<double>(f),static_cast<double>(qc), OFIQ::QualityMeasureReturnCode::Success };
#endif
    }
}