/**
 * @file image_io.h
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
 * @brief Provides helper functions for reading/writing images from/to disk.
 * @author OFIQ development team
 */
#ifndef OFIQ_LIB_IMAGE_IO_H
#define OFIQ_LIB_IMAGE_IO_H

#include "ofiq_lib.h"

 /**
  * Namespace for OFIQ implementations.
  */
namespace OFIQ_LIB {

	/**
	 * @brief Read image from disk.
	 * 
	 * @param[in] filename Path and file name of the image being read from disk.
	 * @param[out] image Reference to the image object where the data is loaded to.
	 * @return OFIQ::ReturnStatus
	 */
	OFIQ_EXPORT OFIQ::ReturnStatus
		readImage(const std::string& filename, OFIQ::Image& image);
}

#endif