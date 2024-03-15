/**
 * @file ONNXRTSegmentation.h
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
 * @brief Helper class to manage the ONNXRuntime session object.
 * @author OFIQ development team
 */
#pragma once

#include <vector>

#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>

/**
 * @brief Helper class to manage the ONNXRuntime session object.
 * @details Helper class to manage the ONNXRuntime session object. Details can be found on the ONNXRuntime documentation
 * https://onnxruntime.ai/docs/get-started/with-cpp.html.
 */
class ONNXRuntimeSegmentation
{
private:

    /**
     * @brief Handle to the ONNXRuntime environment variable.
     * 
     */
    Ort::Env m_ortenv;

    /**
     * @brief ONNXRuntime variable to setup the tensors used in ONNXRuntime.
     * 
     */
    Ort::MemoryInfo m_memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    
    /**
     * @brief Description of the shape of the input data expected by the model.
     * 
     */
    std::array<int64_t, 4> m_inputShape;

    /**
     * @brief Handle to the ONNXRuntime session.
     * 
     */
    std::unique_ptr<Ort::Session> m_ort_session;

    /**
     * @brief Private method to generate an ONNXRuntime session object.
     * 
     * @param i_model_data Model data loaded from file.
     * @param i_imageWidth Width of the input image as expected by the model.
     * @param i_imageHeight Height of the input image as expected by the model.
     */
    void init_session(const std::vector<uint8_t>& i_model_data, int64_t i_imageWidth, int64_t i_imageHeight);
 

public:
    /**
     * @brief Constructor
     * 
     */
    ONNXRuntimeSegmentation() = default;

    /**
     * @brief Destructor
     * 
     */
    ~ONNXRuntimeSegmentation() = default;

    /**
     * @brief Public method to generate an ONNXRuntime session object.
     * 
     * @param i_modelData Model data loaded from file.
     * @param i_imageWidth Width of the input image as expected by the model.
     * @param i_imageHeight Height of the input image as expected by the model.
     */
    void initialize(
        const std::vector<uint8_t>& i_modelData, int64_t i_imageWidth, int64_t i_imageHeight);
    
    /**
     * @brief Get the number of output nodes (results) based on the loaded model.
     * 
     * @return size_t number of output nodes (results).
     */
    size_t getNumberOfOutputNodes();

    /**
     * @brief Perform the computation.
     * 
     * @param i_netInput Input to the neural net.
     * @return std::vector<Ort::Value> Result of the neural net computation.
     */
    std::vector<Ort::Value> run( std::vector<float>&  i_netInput);
    
};
