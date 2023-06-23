#pragma once
#include "faceqa_lib.h"

namespace FACEQA_LIB
{
    class FaceQaError : public std::exception
    {
    public:
        FaceQaError(FACEQA::ReturnCode returnCode, std::string message)
            : returnCode{returnCode},
              message{message},
              msgbuf{""}
        {
        }

        const char* what() const noexcept override;

        FACEQA::ReturnCode whatCode() const noexcept { return returnCode; }

    private:
        FACEQA::ReturnCode returnCode;
        std::string message;
        char msgbuf[512];
    };
}