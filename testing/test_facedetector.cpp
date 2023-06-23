#include <fstream>
#include <filesystem>
#include <cstdlib>

#include <tao/json.hpp>
#include <gtest/gtest.h>

#include <faceqa_lib.h>

#include <Configuration.h>
#include <opencv_ssd_face_detector.h>
#include <Session.h>
#include <image_io.h>

#include "test_constants.h"

using namespace FACEQA_LIB;
using namespace FACEQA_LIB::modules::detectors;

namespace fs = std::filesystem;

using jType = tao::json::type;


TEST(Facefinder, FindFace)
{
  /* Get implementation pointer */
  std::string configDir, configFile;
  configFile = FACEQA_LIB_CONFIG_FILE_SSD_FACE;
  configDir = FACEQA_LIB_CONFIG_DIR;

  fs::path fullConfPath, pathConfFilename, pathConfDir(configDir);
  // use default fileName 'faceqa.jaxn' if no configValue was given
  pathConfFilename = fs::path(configFile);
  fullConfPath = fs::weakly_canonical(pathConfDir / pathConfFilename);

  std::ifstream istream(fullConfPath.string());

  std::string source;
  tao::json::value jsonValue = tao::json::jaxn::from_stream(istream, source);

  Configuration config(configDir, configFile);

  SSDFaceDetector faceDetector(config);

  // bounding box
  unsigned int bb_xleft =
      jsonValue["Test.SSDFaceDetector"]["face_bounding_box"]["xleft"].get_unsigned();
  unsigned int bb_ytop =
      jsonValue["Test.SSDFaceDetector"]["face_bounding_box"]["ytop"].get_unsigned();
  unsigned int bb_width =
      jsonValue["Test.SSDFaceDetector"]["face_bounding_box"]["width"].get_unsigned();
  unsigned int bb_height =
      jsonValue["Test.SSDFaceDetector"]["face_bounding_box"]["height"].get_unsigned();


  std::string inputImageFile = jsonValue["Test.SSDFaceDetector"]["input_image"].get_string();

  FACEQA::Image inputImage;
  FACEQA::ReturnStatus ret = readImage(inputImageFile, inputImage);

  EXPECT_TRUE(ret.code == FACEQA::ReturnCode::Success);

  FACEQA::FaceImageQualityAssessment assessment;

  Session session(inputImage, assessment);

  std::vector<FACEQA::BoundingBox> foundFaces = faceDetector.detectFaces(session);

  EXPECT_TRUE(foundFaces.size() == 2);
  EXPECT_TRUE(foundFaces[0].faceDetector == FACEQA::FaceDetectorType::OPENCVSSD);

  EXPECT_EQ(foundFaces[0].xleft, bb_xleft);
  EXPECT_EQ(foundFaces[0].ytop, bb_ytop);
  EXPECT_EQ(foundFaces[0].width, bb_width);
  EXPECT_EQ(foundFaces[0].height, bb_height);

}
