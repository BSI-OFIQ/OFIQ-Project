/**
 * @file BackgroundUniformity.cpp
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

#include "BackgroundUniformity.h"
#include "OFIQError.h"
#include "utils.h"
#include "image_utils.h"

namespace OFIQ_LIB::modules::measures
{
    static const auto qualityMeasure = OFIQ::QualityMeasure::BackgroundUniformity;

    static cv::Mat GetGradients(const cv::Mat& L);

    BackgroundUniformity::BackgroundUniformity(
        const Configuration& configuration,
        OFIQ_LIB::Session& session)
        : Measure{ configuration, session, qualityMeasure }
    {
        SigmoidParameters defaultValues;
        defaultValues.h = 100.0;
        defaultValues.x0 = 10;
        defaultValues.w = 30;
        defaultValues.setInverse();
        defaultValues.round = true;
        AddSigmoid(qualityMeasure, defaultValues);
    }

    void BackgroundUniformity::Execute(OFIQ_LIB::Session & session)
    {
        // Input: Aligned image I
        auto I = session.getAlignedFace();

        // Input: Transformation T
        auto T = session.getAlignedFaceTransformationMatrix();

        // Input: face parsing segmentation map S
        auto S = session.getFaceParsingImage();

        // Input: dimensions (w,h) of the original image
        auto h = session.image().height;
        auto w = session.image().width;

        // Step 1. Create a completely black (0) grey scale image A of dimensions (w,h)
        auto A = cv::Mat::zeros(h,w,CV_8U);

        // Step 2. Apply the transformation T to A and/or pad the image
        // to dimension 616x616 to obtain a padding mask P. For padding use white colour (255)
        cv::Mat P = cv::Mat::zeros(I.rows,I.cols,CV_8U);
        cv::warpAffine(A, P, T, cv::Size(I.cols, I.rows), cv::INTER_NEAREST, 0, 255);

        // Step 3. Crop both I and P by 62 pixels from both sides and by 108 pixels from the bottom.
        I = cv::Mat(I, cv::Range(m_crop_top,I.rows-m_crop_bottom),cv::Range(m_crop_left,I.cols-m_crop_right));
        P = cv::Mat(P, cv::Range(m_crop_top,P.rows-m_crop_bottom),cv::Range(m_crop_left,P.cols-m_crop_right));

        // Step 4. Resize both I and P to size (354,295)
        cv::resize(I, I, cv::Size(m_target_width, m_target_height), 0.0, 0.0, cv::INTER_LINEAR);
        cv::resize(P, P, cv::Size(m_target_width, m_target_height), 0.0, 0.0, cv::INTER_NEAREST);

        // Step 5. Crop the segmentation map S by 23 pixels from both sides and 108 pixels from the bottom
        auto marginX = (S.cols-I.cols)/2; // marginX shall be 23 as per ISO/IEC 29794-5
        S = cv::Mat(S, cv::Range(0, I.rows), cv::Range(marginX, S.cols-marginX));

        // Step 6. Compute the background mask B with Bij=1, if Sij=0 and Pij=0, and Bij otherwise
        cv::Mat B = cv::Mat::zeros(P.rows,P.cols, CV_8U);
        for (int i = 0; i < P.rows; i++)
        {
            for (int j = 0; j < P.cols; j++)
            {
                if (P.at<uchar>(i,j) == 0 && S.at<uchar>(i,j) == 0)
                {
                    B.data[B.cols*i+j] = 1;
                }
            }
        }



        // Step 7. Apply to B the OpenCV function erode with kernel size 4.
        cv::Mat kernel = cv::Mat::ones(m_erosion_kernel_size, m_erosion_kernel_size, CV_8U);
        cv::erode(B, B, kernel, cv::Point(-1, -1), 1);

        // if B has only zeroes, i.e. the background mask is empty,
        // can break at this point
        int nNonZeroBg = cv::countNonZero(B);
        if ( nNonZeroBg == 0)
            return SetQualityMeasure(session, qualityMeasure, 0, OFIQ::QualityMeasureReturnCode::FailureToAssess);

        // Step 8. Compute the luminance image L for image I as specified in ISO/IEC CD2 29794-5:2023 [1].
        // Each pixel value is encoded as an integer value between 0 (black) and 255 (white)
        auto L = GetLuminanceImageFromBGR(I);

        // Step 9. Algorithm 2 (Luminance Gradients)
        auto G = GetGradients(L);
        double m = 0.0;
        int n = 0;
        for (int i = 0; i < G.rows; i++)
        {
            for (int j = 0; j < G.cols; j++)
            {
                if (B.at<uchar>(i, j))
                {
                    m += G.at<double>(i, j);
                    ++n;
                }
            }
        }

        if (n > 0)
            m /= (double)n;

        SetQualityMeasure(session, qualityMeasure, m, OFIQ::QualityMeasureReturnCode::Success);
    }

    static cv::Mat GetGradients(const cv::Mat& L)
    {
        cv::Mat sX;
        cv::Mat sY;

        cv::Sobel(L, sX, CV_32F,1, 0, -1);
        cv::Sobel(L, sY, CV_32F,0, 1, -1);

        cv::Mat G = cv::Mat::zeros(L.rows, L.cols, CV_64F);
        for (int i = 0; i < L.rows; i++)
        {
            for (int j = 0; j < L.cols; j++)
            {
                auto& v = G.at<double>(i, j);
                auto sx = (double)sX.at<float>(i, j);
                auto sy = (double)sY.at<float>(i, j);
                v = (double)sqrt(sx * sx + sy * sy);
            }
        }

        return G;
    }
}