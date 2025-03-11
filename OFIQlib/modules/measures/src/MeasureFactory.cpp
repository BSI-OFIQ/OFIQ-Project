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
    static const std::map<OFIQ::QualityMeasure, std::function<std::unique_ptr<Measure>(const Configuration&)>> factoryMapping
    {
        {OFIQ::QualityMeasure::SingleFacePresent, [](const Configuration& configuration) { return std::make_unique<SingleFacePresent>(configuration); }},
        {OFIQ::QualityMeasure::HeadPose, [](const Configuration& configuration) { return std::make_unique<HeadPose>(configuration); }},
        {OFIQ::QualityMeasure::HeadPoseRoll, [](const Configuration& configuration) { return std::make_unique<HeadPose>(configuration); }},
        {OFIQ::QualityMeasure::HeadPosePitch, [](const Configuration& configuration) { return std::make_unique<HeadPose>(configuration); }},
        {OFIQ::QualityMeasure::HeadPoseYaw, [](const Configuration& configuration) { return std::make_unique<HeadPose>(configuration); }},
        {OFIQ::QualityMeasure::UnderExposurePrevention, [](const Configuration& configuration) { return std::make_unique<UnderExposurePrevention>(configuration); }},
        {OFIQ::QualityMeasure::OverExposurePrevention, [](const Configuration& configuration) { return std::make_unique<OverExposurePrevention>(configuration); }},
        {OFIQ::QualityMeasure::BackgroundUniformity, [](const Configuration& configuration) { return std::make_unique<BackgroundUniformity>(configuration); }},
        {OFIQ::QualityMeasure::MouthOcclusionPrevention, [](const Configuration& configuration) { return std::make_unique<MouthOcclusionPrevention>(configuration); }},
        {OFIQ::QualityMeasure::MouthClosed, [](const Configuration& configuration) { return std::make_unique<MouthClosed>(configuration); }},
        {OFIQ::QualityMeasure::ExpressionNeutrality, [](const Configuration& configuration) { return std::make_unique<ExpressionNeutrality>(configuration); }},
        {OFIQ::QualityMeasure::EyesOpen, [](const Configuration& configuration) { return std::make_unique<EyesOpen>(configuration); }},
        {OFIQ::QualityMeasure::EyesVisible, [](const Configuration& configuration) { return std::make_unique<EyesVisible>(configuration); }},
        {OFIQ::QualityMeasure::FaceOcclusionPrevention, [](const Configuration& configuration) { return std::make_unique<FaceOcclusionPrevention>(configuration); }},
        {OFIQ::QualityMeasure::IlluminationUniformity, [](const Configuration& configuration) { return std::make_unique<IlluminationUniformity>(configuration); }},
        {OFIQ::QualityMeasure::InterEyeDistance, [](const Configuration& configuration) { return std::make_unique<InterEyeDistance>(configuration); }},
        {OFIQ::QualityMeasure::Luminance, [](const Configuration& configuration) { return std::make_unique<Luminance>(configuration); }},
        {OFIQ::QualityMeasure::LuminanceMean, [](const Configuration& configuration) { return std::make_unique<Luminance>(configuration); }},
        {OFIQ::QualityMeasure::LuminanceVariance, [](const Configuration& configuration) { return std::make_unique<Luminance>(configuration); }},
        {OFIQ::QualityMeasure::DynamicRange, [](const Configuration& configuration) { return std::make_unique<DynamicRange>(configuration); }},
        {OFIQ::QualityMeasure::CropOfTheFaceImage, [](const Configuration& configuration) { return std::make_unique<CropOfTheFaceImage>(configuration); }},
        {OFIQ::QualityMeasure::LeftwardCropOfTheFaceImage, [](const Configuration& configuration) { return std::make_unique<CropOfTheFaceImage>(configuration); }},
        {OFIQ::QualityMeasure::RightwardCropOfTheFaceImage, [](const Configuration& configuration) { return std::make_unique<CropOfTheFaceImage>(configuration); }},
        {OFIQ::QualityMeasure::MarginBelowOfTheFaceImage, [](const Configuration& configuration) { return std::make_unique<CropOfTheFaceImage>(configuration); }},
        {OFIQ::QualityMeasure::MarginAboveOfTheFaceImage, [](const Configuration& configuration) { return std::make_unique<CropOfTheFaceImage>(configuration); }},
        {OFIQ::QualityMeasure::NaturalColour, [](const Configuration& configuration) { return std::make_unique<NaturalColour>(configuration); }},
        {OFIQ::QualityMeasure::CompressionArtifacts, [](const Configuration& configuration) { return std::make_unique<CompressionArtifacts>(configuration); }},
        {OFIQ::QualityMeasure::HeadSize, [](const Configuration& configuration) { return std::make_unique<HeadSize>(configuration); }},
        {OFIQ::QualityMeasure::UnifiedQualityScore, [](const Configuration& configuration) { return std::make_unique<UnifiedQualityScore>(configuration); }},
        {OFIQ::QualityMeasure::NoHeadCoverings, [](const Configuration& configuration) { return std::make_unique<NoHeadCoverings>(configuration); }},
        {OFIQ::QualityMeasure::Sharpness, [](const Configuration& configuration) { return std::make_unique<Sharpness>(configuration); }}
    };
    
    std::unique_ptr<Measure> MeasureFactory::CreateMeasure(
        const OFIQ::QualityMeasure measure,
        const Configuration& configuration)
    {   
        auto it = factoryMapping.find(measure);
        if (it != factoryMapping.end())
        {
            return it->second(configuration);
        }
        else
        {
            return nullptr;
        }
    }
}