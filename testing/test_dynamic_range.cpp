#include <fstream>
#include <filesystem>
#include <cstdlib>

#include <tao/json.hpp>
#include <gtest/gtest.h>

#include <faceqa_lib.h>

#include <Configuration.h>
#include <opencv_ssd_face_detector.h>
#include <AllLandmarks.h>
#include <Session.h>
#include <image_io.h>

#include "test_constants.h"


using namespace FACEQA_LIB;
using namespace FACEQA_LIB::modules::detectors;
using namespace FACEQA_LIB::modules::landmarks;

namespace fs = std::filesystem;

TEST(DynamicRange, WD5)
{
  /* Get implementation pointer */
  std::string configDir, configFile;
  configFile = FACEQA_LIB_CONFIG_FILE_MEASURES;
  configDir = FACEQA_LIB_CONFIG_DIR;

  fs::path fullConfPath, pathConfFilename, pathConfDir(configDir);
  // use default fileName 'faceqa.jaxn' if no configValue was given
  pathConfFilename = fs::path(configFile);
  fullConfPath = fs::weakly_canonical(pathConfDir / pathConfFilename);

  std::ifstream istream(fullConfPath.string());

  std::string source;
  tao::json::value jsonValue = tao::json::jaxn::from_stream(istream, source);

  // Get implementation pointer
  auto implPtr = Interface::getImplementation();
  // Initialization
  FACEQA::ReturnStatus ret = implPtr->initialize(configDir,configFile);
  EXPECT_EQ(ret.code, FACEQA::ReturnCode::Success);

  // get expected results from the test configuration
  std::string inputImageFile = jsonValue["Test.DynamicRange"]["input_image"].get_string();
  auto rawScore = jsonValue["Test.DynamicRange"]["raw"].get_double();
  auto scalarScore = jsonValue["Test.DynamicRange"]["scalar"].get_double();

  // Read input image
  FACEQA::Image inputImage;
  ret = readImage(inputImageFile, inputImage);
  EXPECT_EQ(ret.code, FACEQA::ReturnCode::Success);
  
  // Run quality assessment
  FACEQA::FaceImageQualityAssessment assessments;
  Session session(inputImage, assessments);
  ret = implPtr->vectorQuality(inputImage, assessments);
  EXPECT_EQ(ret.code, FACEQA::ReturnCode::Success);

  // Get tested measure result
  std::optional<FACEQA::QualityMeasureResult> result;
  for (const auto& qaResult : assessments.qAssessments)
  {
      if (qaResult.first == FACEQA::QualityMeasure::DynamicRange )
      {
          result = qaResult.second;
          break;
      }
  }
  EXPECT_TRUE(result.has_value());

  // Test on agreement with expected results
  EXPECT_EQ(result.value().rawScore, rawScore);
  EXPECT_EQ(result.value().scalar, scalarScore);
}
