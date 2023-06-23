#include "Session.h"

namespace FACEQA_LIB
{
    static int sessionCounter = 0;

    std::string Session::GenerateId()
    {
        return std::to_string(++sessionCounter);
    }
}