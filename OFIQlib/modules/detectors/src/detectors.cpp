/**
 * @file detectors.cpp
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

#include "detectors.h"
#include "utils.h"
#include <algorithm>

namespace OFIQ_LIB
{
    std::vector<OFIQ::BoundingBox>
        FaceDetectorInterface::detectFaces(OFIQ_LIB::Session& session)
    {
        auto faces = UpdateFaces(session);
        if (faces.size() == 0)
            return faces;

        // sort by face box area, so that biggest face will be on the first index.
        std::sort(
            faces.begin(),
            faces.end(),
            [](OFIQ::BoundingBox rh, OFIQ::BoundingBox lh) { return rh.width * rh.height > lh.width * lh.height; });
        
        session.assessment().boundingBox = faces[0];
        return faces;
    }
}