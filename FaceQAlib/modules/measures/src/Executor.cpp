#include "Executor.h"

namespace FACEQA_LIB::modules::measures
{

    void Executor::ExecuteAll()
    {
        for (auto& measure : measures)
            measure->Execute();
    }
}