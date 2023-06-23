#pragma once

#include "Measure.h"

namespace FACEQA_LIB::modules::measures
{
    class Executor
    {
    public:
        Executor(std::vector<std::unique_ptr<Measure>> measures)
            : measures{std::move(measures)}
        {
        }

        void ExecuteAll();

    private:
        std::vector<std::unique_ptr<Measure>> measures;
    };
}