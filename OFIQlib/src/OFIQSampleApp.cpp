/**
 * @file OFIQSampleApp.cpp
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

#if defined _WIN32 && defined OFIQ_EXPORTS
#undef OFIQ_EXPORTS
#endif

#include "ofiq_lib.h"
#include "image_io.h"
#include "utils.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <magic_enum.hpp>
#include <filesystem>
#include <chrono>

constexpr int SUCCESS = 0;
constexpr int FAILURE = 1;


namespace fs = std::filesystem;

using namespace std;
using namespace OFIQ;
using namespace OFIQ_LIB;


int getQualityAssessmentResults(
    const std::shared_ptr<Interface>& implPtr,
    const string& inputFile,
    FaceImageQualityAssessment& assessments, int & r_elapsed );

std::vector<std::string> readFileLines(
    const std::string& inputFile);

string exportAssessmentResultsToString(
    const FaceImageQualityAssessment& assessments,
    bool doExportScalar = false);

std::vector<std::string> readFileLines(const std::string& inputFile)
{
    std::vector<std::string> filenames;
    std::ifstream ifs(inputFile);
    std::string line;

    while (ifs)
    {
        std::getline(ifs, line);
        // ignore empty lines and comment lines
        if (line.empty() || line.at(0) == '#')
            continue;
        filenames.push_back(line);
    }

    return filenames;
}

std::string to_lower(std::string data)
{
    std::transform(data.begin(), data.end(), data.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return data;
}

std::vector<std::string> readImageFilesFromDirectory(const std::string& inputDirectory)
{
    std::vector<std::string> filenames;

    for (const auto& entry : fs::directory_iterator(inputDirectory))
    {
        std::string extension = to_lower(entry.path().extension().generic_string());
        if (extension == ".jpg" || extension == ".jpeg" || extension == ".png")
        {
            filenames.push_back(entry.path().generic_string());
        }
    }

    return filenames;
}

bool isStringContained(
    const std::vector<std::string>& strings, 
    const std::string& str,
    bool isCaseSensitive=false)
{
    std::string s = str;
    if (!isCaseSensitive)
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return std::tolower(c); });

    return (!s.empty() &&
        std::find(strings.begin(), strings.end(), s) != strings.end());
}

int runQuality(
    const std::shared_ptr<Interface>& implPtr,
    const fs::path& inputFile,
    std::ostream* outStreamPtr = &std::cout,
    bool doConsoleOut = false)
{
    std::vector<std::string> imageFiles;
    std::vector<FaceImageQualityAssessment> faceImageQAs;
    std::vector<int> faceImageQAresultCodes;
    std::vector<int> faceImageQAassessmentTimes;

    if (fs::is_directory(fs::path(inputFile)))
    {
        imageFiles = readImageFilesFromDirectory(inputFile.generic_string());
    }
    else if (std::string fileExt = inputFile.extension().string();
        isStringContained({ ".txt", ".csv" }, fileExt))
        // a list of image files
        imageFiles = readFileLines(inputFile.generic_string());
    else
        // single image file
        imageFiles.push_back(inputFile.generic_string());

    // process image file(s)
    constexpr bool EXPORT_RAW = false;
    constexpr bool EXPORT_SCALAR = true;
    bool outputHeaderIn1stIter = true;
    for (auto const& imageFile: imageFiles)
    {
        FaceImageQualityAssessment assessmentResult;

        int time_elapsed_ms = 0;
        int resCode = getQualityAssessmentResults(implPtr, imageFile, assessmentResult, time_elapsed_ms);
        faceImageQAresultCodes.push_back(resCode);
        faceImageQAassessmentTimes.push_back(time_elapsed_ms);

        faceImageQAs.push_back(assessmentResult);

        string strQAresRaw = exportAssessmentResultsToString(
            assessmentResult, EXPORT_RAW);
        string strQAresScalar = exportAssessmentResultsToString(
            assessmentResult, EXPORT_SCALAR);

        // output result of each file right after it was processed
        if (outputHeaderIn1stIter)
        {
            // print the header. the format is the following
            // "Filename", MeasurementName1, ..., MeasurementNameN, MeasurementName1.scalar, ..., MeasurementNameN.scalar
            // Filename,      Measurement1.raw, ..., MeasurementN.raw, Measurement1.scalar, ..., MeasurementN.scalar
            vector<string> measureNames;
            vector<string> measureNamesScalar;
            for (const auto& [measure, measure_result] : faceImageQAs[0].qAssessments)
            {
                auto mName = static_cast<std::string>(magic_enum::enum_name(measure));
                measureNames.push_back(mName);
                measureNamesScalar.push_back(mName + string(".scalar"));
            }

            *outStreamPtr << "Filename;";
            for (const auto& mn : measureNames)
                *outStreamPtr << mn << ';';
            for (const auto& mn : measureNamesScalar)
                *outStreamPtr << mn << ';';
            *outStreamPtr << "assessment_time_in_ms;" << std::endl;

            outputHeaderIn1stIter = false;
        }

        *outStreamPtr << imageFile << ';' << strQAresRaw << ';' << strQAresScalar << ';' << time_elapsed_ms << std::endl;

        if (doConsoleOut)
        {
            std::cout << "-------------------------------------------------------" << std::endl;
            std::cout << "Image file: '" << imageFile << "' has attributes:" << std::endl;
            for (const auto& [measure, measure_result] : assessmentResult.qAssessments)
            {
                auto mName = static_cast<std::string>(magic_enum::enum_name(measure));
                auto rawScore = measure_result.rawScore;
                auto scalarScore = measure_result.scalar;
                if (measure_result.code != QualityMeasureReturnCode::Success)
                    scalarScore = -1;
                std::cout << mName << "-> rawScore:  " << rawScore << "   scalar: " << scalarScore
                    << std::endl;
            }
            std::cout << "-------------------------------------------------------" << std::endl;
        }
    }

    if (faceImageQAs.empty())
    {
        cerr << "[ERROR] " << "empty result list" << "." << endl;
        return FAILURE;
    }

    if (faceImageQAs.size() != imageFiles.size())
    {
        cerr << "[ERROR] " << "invalid number of measurement results. Is " << imageFiles.size() <<
            ", has to be " << faceImageQAs.size() << endl;
        return FAILURE;
    }

    return SUCCESS;
}

int getQualityAssessmentResults(
    const std::shared_ptr<Interface>& implPtr,
    const string& inputFile,
    FaceImageQualityAssessment& assessments,
    int & r_elapsed)
{
    Image image;
    ReturnStatus retStatus = readImage(inputFile, image);

    if (retStatus.code != ReturnCode::Success)
    {
        cerr << "[ERROR] " << retStatus.info << "." << endl;
        return FAILURE;
    }

    //std::cout << "--> Start processing image file: " << inputFile << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    retStatus = implPtr->vectorQuality(image, assessments);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    r_elapsed = static_cast<int>(elapsed.count());

    return retStatus.code == ReturnCode::Success ? SUCCESS : FAILURE;
}

string exportAssessmentResultsToString(
    const FaceImageQualityAssessment& assessments, 
    bool doExportScalar)
{
    std::string resultStr;
    for ( auto it = assessments.qAssessments.begin() ; it != assessments.qAssessments.end() ; )
    {
        const auto& [measure, measure_result] = *it;
        const QualityMeasureResult& qaResult = measure_result;

        double scalarScore;
        if (measure_result.code != QualityMeasureReturnCode::Success)
        {
            scalarScore = -1.0;
        }
        else
        {
            scalarScore = qaResult.scalar;
        }

        if (double val = doExportScalar ? scalarScore : qaResult.rawScore;
            round(val) == val)
        {
            resultStr += to_string((int)val);
        }
        else
            resultStr += to_string(val);

        if (++it != assessments.qAssessments.end())
        {
            resultStr += ';';
        }
    }
    // replace decimal point for german MS Excel
    //std::replace(resultStr.begin(), resultStr.end(), '.', ',');    
    return resultStr;
}


void usage(const string& executable)
{
    cerr << "Usage: " << executable
         << " -c configDir "
            "-o outputFile -h outputStem -i inputFile -cf configFile"
         << endl;
}

int main(int argc, char* argv[])
{
    int requiredArgs = 1; /* exec name */
    if (argc < requiredArgs)
    {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    fs::path configDir("config");
    const char* outputFile = nullptr;
    fs::path inputFile;
    fs::path configFile;

    int i = 0;
    while (i < argc - requiredArgs)
    {
        if (strcmp(argv[requiredArgs + i], "-c") == 0)
            configDir = fs::path(argv[requiredArgs + (++i)]);
        else if (strcmp(argv[requiredArgs + i], "-o") == 0)
            outputFile = argv[requiredArgs + (++i)];
        else if (strcmp(argv[requiredArgs + i], "-i") == 0)
            inputFile = fs::path(argv[requiredArgs + (++i)]);
        else if (strcmp(argv[requiredArgs + i], "-cf") == 0)
            configFile = fs::path(argv[requiredArgs + (++i)]);
        else
        {
            cerr << "[ERROR] Unrecognized flag: " << argv[requiredArgs + i] << endl;
            usage(argv[0]);
            return FAILURE;
        }
        ++i;
    }

    if (fs::is_regular_file(configDir))
    {
        if (!configFile.empty())
        {
            cerr << "[ERROR] Redundant specification of configuration file." << endl;
            return FAILURE;
        }

        configFile = fs::path(configDir).filename();
        configDir = fs::path(configDir).parent_path();
    }

    /* Get implementation pointer */
    auto implPtr = Interface::getImplementation();
    /* Initialization */
    auto start_time = std::chrono::high_resolution_clock::now();
    auto ret = implPtr->initialize(
        configDir.generic_string(),
        configFile.generic_string());
    auto end_time = std::chrono::high_resolution_clock::now();

    if (ret.code != ReturnCode::Success)
    {
        cerr << "[ERROR] initialize() returned error: " << ret.code << "." << endl
             << ret.info << endl;
        return FAILURE;
    }

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "[INFO] Initialization took: " << elapsed.count() << "ms" << std::endl;

    int major;
    int minor;
    int patch;
    implPtr->getVersion(major, minor, patch);
    
    cout << "OFIQ library version: " << major << '.' << minor << '.' << patch << endl;

    // write to output file
    if (outputFile != nullptr)
    {
        std::ofstream ofs(outputFile);
        if (ofs.good())
        {
            runQuality(implPtr, inputFile, &ofs);
        }
        else
        {
        cerr << "[ERROR] Could not open '" << outputFile << "'." << endl
             << ret.info << endl;
        return FAILURE;
        }
    }
    else
    {
        runQuality(implPtr, inputFile, &std::cout);
    }

    return 0;
}
