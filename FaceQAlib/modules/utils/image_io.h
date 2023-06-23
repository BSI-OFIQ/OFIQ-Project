#ifndef FACEQA_LIB_IMAGE_IO_H
#define FACEQA_LIB_IMAGE_IO_H

#include "faceqa_lib.h"

namespace FACEQA_LIB {

	FACEQA_EXPORT FACEQA::ReturnStatus
		readImage(const std::string& filename, FACEQA::Image& image);
}

#endif