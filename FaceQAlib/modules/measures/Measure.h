#pragma once

#include "Configuration.h"
#include "faceqa_lib.h"
#include "Session.h"
#ifndef _WIN32
#    include <math.h>
#endif

namespace FACEQA_LIB::modules::measures
{
    struct SigmoidParameters
    {
        SigmoidParameters() { Reset(); }

        double h;
        double a;
        double s;
        double x0;
        double w;
        bool round;

        void setInverse()
        {
            a = 1;
            s = -1;
        }

        void Reset()
        {
            h = 100;
            a = 0;
            s = 1;
            x0 = 4;
            w = 0.7;
            round = true;
        }
    };

    class Measure
    {
    public:
        Measure(const Configuration& configuration, FACEQA_LIB::Session& session)
            : configuration{configuration},
              session{session}
        {
            configuration.GetBool(Configuration::Tags::generateDebugImages, generateDebugImages);
        }

        virtual void Execute() = 0;
        virtual ~Measure() = default;

    protected:
        static double Sigmoid(double x, double x0, double w)
        {
            return 1.0 / (1 + exp((x0 - x) / w));
        }

        void AddSigmoid(FACEQA::QualityMeasure measure, const SigmoidParameters& defaultValues);
        void AddSigmoid(const std::string& key, SigmoidParameters defaultValues);
        double ExecuteScalarConversion(FACEQA::QualityMeasure measure, double rawValue);
        double ExecuteScalarConversion(const std::string& key, double rawValue);
        void SetQualityMeasure(FACEQA::QualityMeasure measure, double rawValue);

        const Configuration& configuration;
        FACEQA_LIB::Session& session;
        bool generateDebugImages = false;

    private:
        static double ScalarConversion(double rawValue, const SigmoidParameters& par)
        {
            double scalarScore = par.h * (par.a + par.s * Sigmoid(rawValue, par.x0, par.w));
            if (par.round)
                scalarScore = round(scalarScore);
            return scalarScore;
        }

        static inline const std::string debugImagesConfigKey = "generateDebugImages";
        std::map<std::string, SigmoidParameters, std::less<>> sigmoidMap;
        static std::string GetKey(FACEQA::QualityMeasure measure);
        static std::string ExpandKey(std::string_view rawKey);
    };
}