/**
 * @file ONNXRTSegmentation.cpp
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

#include <ONNXRTSegmentation.h>
#include "OFIQError.h"

void ONNXRuntimeSegmentation::initialize(
    const std::vector<uint8_t>& i_modelData, int64_t i_imageWidth, int64_t i_imageHeight)
{

    try
    {
        init_session(i_modelData, i_imageWidth, i_imageHeight);
    }
    catch (std::exception e)
    {
        throw OFIQ_LIB::OFIQError(
            OFIQ::ReturnCode::MissingConfigParamError,
            std::string("Failed loading model for ONNXRuntimeSegmentation."));
    }
}


size_t ONNXRuntimeSegmentation::getNumberOfOutputNodes()
{
    return m_ort_session->GetOutputCount();
}

std::vector<Ort::Value> ONNXRuntimeSegmentation::run( std::vector<float>& i_netInput) {

    std::vector<Ort::Value> results;
            // define names
    Ort::AllocatorWithDefaultOptions ort_alloc;
    Ort::AllocatedStringPtr inputName = m_ort_session->GetInputNameAllocated(0, ort_alloc);
    std::array<const char*, 1> inputNames = {inputName.get()};
    std::vector<const char*> outputNames;
    const size_t num_output_nodes = m_ort_session->GetOutputCount();
    for (int i = 0; i < num_output_nodes; i++)
    {
        Ort::AllocatedStringPtr outputName = m_ort_session->GetOutputNameAllocated(i, ort_alloc);
        outputNames.push_back(outputName.get());
        outputName.release();
    }

    inputName.release();


    // define Tensor
    auto inputTensor = Ort::Value::CreateTensor<float>(
        m_memory_info,
        i_netInput.data(),
        i_netInput.size(),
        m_inputShape.data(),
        m_inputShape.size());


    // run inference
    Ort::RunOptions runOptions;
    results = m_ort_session->Run(
        runOptions,
        inputNames.data(),
        &inputTensor,
        1,
        outputNames.data(),
        num_output_nodes);

    return results;
}

void ONNXRuntimeSegmentation::init_session(
    const std::vector<uint8_t>& i_model_data,
    int64_t i_imageWidth,
    int64_t i_imageHeight)
{
    m_ortenv = Ort::Env(ORT_LOGGING_LEVEL_ERROR);
    m_ort_session = std::make_unique<Ort::Session>(
        m_ortenv,
        i_model_data.data(),
        i_model_data.size(),
        Ort::SessionOptions{nullptr});


    auto type_info = m_ort_session->GetInputTypeInfo(0);
    auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
    auto input_node_shape = tensor_info.GetShape();

    int64_t expected_image_number_of_channels = input_node_shape[1] > 0 ? input_node_shape[1] : 3;
    int64_t expected_image_width = i_imageWidth;
    int64_t expected_image_height = i_imageHeight;

    int64_t number_of_input_elements =
        expected_image_number_of_channels * expected_image_width * expected_image_height;

    // define shape
    std::array<int64_t, 4> inputShape2 =
        {1, expected_image_number_of_channels, expected_image_height, expected_image_width};

    m_inputShape = std::move(inputShape2);
}