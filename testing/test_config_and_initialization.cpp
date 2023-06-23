#include <gtest/gtest.h>

#include <faceqa_lib.h>
#include "test_constants.h"

using namespace FACEQA_LIB;

// Demonstrate some basic assertions.
TEST(FaceQAInterface, LoadEmptyConfig) {

  /* Get implementation pointer */
  std::string configDir, configFile;

  auto implPtr = Interface::getImplementation();

  FACEQA::ReturnStatus ret;
  EXPECT_NO_THROW(
    ret = implPtr->initialize(
        configDir,
        configFile);
  );

  // Expect equality.
  EXPECT_TRUE(ret.code == FACEQA::ReturnCode::UnknownError);
}

TEST(FaceQAInterface, LoadWorkingConfig)
{

  /* Get implementation pointer */
  std::string configDir, configFile;
  configDir = FACEQA_LIB_CONFIG_DIR;

  auto implPtr = Interface::getImplementation();

  FACEQA::ReturnStatus ret;
  EXPECT_NO_THROW(ret = implPtr->initialize(configDir, configFile););

  // Expect equality.
  EXPECT_TRUE(ret.code == FACEQA::ReturnCode::Success);
}
