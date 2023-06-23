
#include "FaceQaError.h"
#include <magic_enum.hpp>

namespace FACEQA_LIB
{
    const char* FaceQaError::what() const noexcept
    {
        auto completeMessage =
            static_cast<std::string>(magic_enum::enum_name(returnCode)) + ": " + message;
        snprintf(const_cast<char*>(msgbuf), 512, "%s", completeMessage.c_str());
        return msgbuf;
    }
}