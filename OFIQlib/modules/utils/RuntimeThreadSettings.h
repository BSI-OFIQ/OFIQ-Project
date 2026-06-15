/**
 * @file RuntimeThreadSettings.h
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
 * @brief Helpers for reading and applying runtime threading configuration.
 * @author OFIQ development team
 */

#pragma once

#include "Configuration.h"

namespace OFIQ_LIB
{
    /**
     * @brief Utility for reading thread limits from the JAXN configuration.
     * @details The following optional top-level configuration keys are supported:
     * <code>opencv_threads</code>, <code>ort_intra_threads</code>, and
     * <code>ort_inter_threads</code>. A value of <code>0</code> keeps the
     * corresponding backend default behavior.
     */
    class RuntimeThreadSettings
    {
    public:
        RuntimeThreadSettings() = delete;
        RuntimeThreadSettings(const RuntimeThreadSettings&) = delete;
        RuntimeThreadSettings(RuntimeThreadSettings&&) = delete;
        RuntimeThreadSettings& operator=(const RuntimeThreadSettings&) = delete;
        RuntimeThreadSettings& operator=(RuntimeThreadSettings&&) = delete;

        /**
         * @brief JAXN key for the OpenCV thread limit.
         */
        inline static constexpr const char* OpenCVThreadsConfigKey = "opencv_threads";

        /**
         * @brief JAXN key for the ONNX Runtime intra-op thread limit.
         */
        inline static constexpr const char* OrtIntraThreadsConfigKey = "ort_intra_threads";

        /**
         * @brief JAXN key for the ONNX Runtime inter-op thread limit.
         */
        inline static constexpr const char* OrtInterThreadsConfigKey = "ort_inter_threads";

        /**
         * @brief Reads a configured thread limit from the JAXN configuration.
         * @param config Configuration instance.
         * @param key Top-level JAXN key to read.
         * @return Configured non-negative integer value, or <code>0</code> if the
         * key is not present.
         * @throws OFIQ_LIB::OFIQError if the configured value is not a
         * non-negative integer.
         */
        static unsigned int GetConfiguredThreadCount(const Configuration& config, const char* key);

        /**
         * @brief Reads the OpenCV thread limit.
         * @param config Configuration instance.
         * @return Configured OpenCV thread limit, or <code>0</code> if not set.
         */
        static unsigned int GetOpenCVThreadCount(const Configuration& config);

        /**
         * @brief Reads the ONNX Runtime intra-op thread limit.
         * @param config Configuration instance.
         * @return Configured intra-op thread limit, or <code>0</code> if not set.
         */
        static unsigned int GetOrtIntraThreadCount(const Configuration& config);

        /**
         * @brief Reads the ONNX Runtime inter-op thread limit.
         * @param config Configuration instance.
         * @return Configured inter-op thread limit, or <code>0</code> if not set.
         */
        static unsigned int GetOrtInterThreadCount(const Configuration& config);

        /**
         * @brief Applies the configured OpenCV thread limit globally.
         * @param config Configuration instance.
         * @details Only the OpenCV backend is changed here. ONNX Runtime thread
         * limits are applied per-session via \link OFIQ_LIB::CreateONNXSessionOptions
         * CreateONNXSessionOptions()\endlink. A configured value of
         * <code>0</code> keeps the OpenCV default behavior, while a configured
         * value of <code>1</code> disables OpenCV internal threading.
         */
        static void ApplyThreadLimit(const Configuration& config);
    };
}
