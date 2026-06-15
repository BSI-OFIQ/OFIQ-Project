/**
 * @file ofiq_lib.h
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
 * @brief Utilities for manipulating the OFIC C API (ofiq_libc) data structures.
 * @author OFIQ development team
 */
#ifndef OFIQ_LIBC_UTILS_H
#define OFIQ_LIBC_UTILS_H

#include <ofiq_structs_c.h>
#include <ofiq_structs.h>
#include <string>
#include <string_view>

const char* getQualityMeasureName(QualityMeasure measure);

ReturnStatus createReturnStatus(ReturnCode code, std::string_view str = "");

ReturnStatus copyImageToCApi(const OFIQ::Image& ofiqImage, Image& capiImage);
ReturnStatus copyImageFromCApi(const Image& capiImage, OFIQ::Image& ofiqImage);

ReturnStatus copyQualityAssessmentToCApi(
    const OFIQ::FaceImageQualityAssessment& ofiqQA,
    FaceImageQualityAssessment& qa);

ReturnStatus copyPreprocessingResultToCApi(
    const OFIQ::FaceImageQualityPreprocessingResult& ofiqPreprocRes,
    const uint16_t imageWidth,
    const uint16_t imageHeight,
    PreprocessingResult& preprocRes);

QualityMeasure QualityMeasureToCApi(
    OFIQ::QualityMeasure measure);

QualityMeasureReturnCode QualityMeasureReturnCodeToCApi(
    OFIQ::QualityMeasureReturnCode qmCode);

#endif /* OFIQ_LIBC_UTILS_H */
