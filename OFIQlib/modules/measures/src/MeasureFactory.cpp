/**
 * @file MeasureFactory.cpp
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

#include "MeasureFactory.h"
#include "AllMeasures.h"

#include <memory>
#include <magic_enum.hpp>

namespace OFIQ_LIB::modules::measures
{

    std::unique_ptr<Measure> MeasureFactory::CreateMeasure(
        const OFIQ::QualityMeasure measure,
        const Configuration& configuration)
    {
        switch (measure)
        {
        case OFIQ::QualityMeasure::SingleFacePresent:
            return std::make_unique<SingleFacePresent>(configuration);
        case OFIQ::QualityMeasure::HeadPose:
        case OFIQ::QualityMeasure::HeadPoseRoll:
        case OFIQ::QualityMeasure::HeadPosePitch:
        case OFIQ::QualityMeasure::HeadPoseYaw:
            return std::make_unique<HeadPose>(configuration);
        case OFIQ::QualityMeasure::UnderExposurePrevention:
            return std::make_unique<UnderExposurePrevention>(configuration);
        case OFIQ::QualityMeasure::OverExposurePrevention:
            return std::make_unique<OverExposurePrevention>(configuration);
        case OFIQ::QualityMeasure::BackgroundUniformity:
            return std::make_unique<BackgroundUniformity>(configuration);
        case OFIQ::QualityMeasure::MouthOcclusionPrevention:
            return std::make_unique<MouthOcclusionPrevention>(configuration);
        case OFIQ::QualityMeasure::MouthClosed:
            return std::make_unique<MouthClosed>(configuration);
        case OFIQ::QualityMeasure::ExpressionNeutrality:
            return std::make_unique<ExpressionNeutrality>(configuration);
        case OFIQ::QualityMeasure::EyesOpen:
            return std::make_unique<EyesOpen>(configuration);
        case OFIQ::QualityMeasure::EyesVisible:
            return std::make_unique<EyesVisible>(configuration);
        case OFIQ::QualityMeasure::FaceOcclusionPrevention:
            return std::make_unique<FaceOcclusionPrevention>(configuration);
        case OFIQ::QualityMeasure::IlluminationUniformity:
            return std::make_unique<IlluminationUniformity>(configuration);
        case OFIQ::QualityMeasure::InterEyeDistance:
            return std::make_unique<InterEyeDistance>(configuration);
        case OFIQ::QualityMeasure::Luminance:
        case OFIQ::QualityMeasure::LuminanceMean:
        case OFIQ::QualityMeasure::LuminanceVariance:
            return std::make_unique<Luminance>(configuration);
        case OFIQ::QualityMeasure::DynamicRange:
            return std::make_unique<DynamicRange>(configuration);
        case OFIQ::QualityMeasure::CropOfTheFaceImage:
        case OFIQ::QualityMeasure::LeftwardCropOfTheFaceImage:
        case OFIQ::QualityMeasure::RightwardCropOfTheFaceImage:
        case OFIQ::QualityMeasure::UpwardCropOfTheFaceImage:
        case OFIQ::QualityMeasure::DownwardCropOfTheFaceImage:
            return std::make_unique<CropOfTheFaceImage>(configuration);
        case OFIQ::QualityMeasure::NaturalColour:
            return std::make_unique<NaturalColour>(configuration);
        case OFIQ::QualityMeasure::CompressionArtifacts:
            return std::make_unique<CompressionArtifacts>(configuration);
        case OFIQ::QualityMeasure::HeadSize:
            return std::make_unique<HeadSize>(configuration);
        case OFIQ::QualityMeasure::UnifiedQualityScore:
            return std::make_unique<UnifiedQualityScore>(configuration);
        case OFIQ::QualityMeasure::NoHeadCoverings:
            return std::make_unique<NoHeadCoverings>(configuration);
        case OFIQ::QualityMeasure::Sharpness:
            return std::make_unique<Sharpness>(configuration);
        default:
            return nullptr;
        }
    }
}