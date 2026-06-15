/**
 * @file ONNXSessionOptions.h
 *
 * @copyright Copyright (c) 2026  Federal Office for Information Security, Germany
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
 * @brief Shared ONNX Runtime session options for configurable threading.
 * @author OFIQ development team
 */

#pragma once

#include "Configuration.h"
#include "RuntimeThreadSettings.h"

#include <onnxruntime_cxx_api.h>

namespace OFIQ_LIB
{
    /**
     * @brief Creates ONNX Runtime session options from the JAXN configuration.
     * @param config Configuration instance.
     * @return Session options configured with
     * <code>ort_intra_threads</code> and <code>ort_inter_threads</code> when
     * those keys are present and non-zero. The execution mode is set according
     * to <code>ort_run_parallel</code> (defaulting to sequential execution).
     */
    inline Ort::SessionOptions CreateONNXSessionOptions(const Configuration& config)
    {
        Ort::SessionOptions options;
        const unsigned int intraThreadCount = RuntimeThreadSettings::GetOrtIntraThreadCount(config);
        const unsigned int interThreadCount = RuntimeThreadSettings::GetOrtInterThreadCount(config);
        if (const bool runParallel = RuntimeThreadSettings::GetOrtRunParallel(config))
            options.SetExecutionMode(ExecutionMode::ORT_PARALLEL);
        else
            options.SetExecutionMode(ExecutionMode::ORT_SEQUENTIAL);
        
        if (intraThreadCount > 0)
            options.SetIntraOpNumThreads(static_cast<int>(intraThreadCount));
        if (interThreadCount > 0)
            options.SetInterOpNumThreads(static_cast<int>(interThreadCount));
        return options;
    }
}
