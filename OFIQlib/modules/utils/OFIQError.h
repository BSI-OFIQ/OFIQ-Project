/**
 * @file OFIQError.h
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
 * @brief Provides a class for the error handling within the QFIQ.
 * @author OFIQ development team
 */
#pragma once
#include "ofiq_lib.h"
#include <string_view>

 /**
  * @brief Namespace for OFIQ implementations.
  */
namespace OFIQ_LIB
{
    /**
     * @brief Implementation of a custom exception.
     * @details This exception is derived from the standard exception. 
     */
    class OFIQError : public std::exception
    {
    public:
        /**
         * @brief Contructor
         * 
         * @param returnCode Based on the OFIQ::ReturnCode (see \link OFIQ::ReturnCode \endlink).
         * @param message Message that will be attached to exception.
         */
        OFIQError(OFIQ::ReturnCode returnCode, std::string_view message);

        /**
         * @brief Getter to the message, overwriting the what method of the base class.
         * 
         * @return const char* Pointer to the extended message.
         */
        const char* what() const noexcept override { return extendedMessage.c_str(); }
        
        /**
         * @brief Getter to the ReturnCode of the QFIQError.
         * 
         * @return OFIQ::ReturnCode 
         */
        OFIQ::ReturnCode whatCode() const noexcept { return returnCode; }

    private:
        /**
         * @brief Member storing the ReturnCode.
         * 
         */
        OFIQ::ReturnCode returnCode;
        
        /**
         * @brief Member, storing the message passed in the constructor.
         * 
         */
        std::string message;

        /**
         * @brief The extended message merges the ReturnCode and the message into one string.
         * 
         */
        std::string extendedMessage;
    };
}