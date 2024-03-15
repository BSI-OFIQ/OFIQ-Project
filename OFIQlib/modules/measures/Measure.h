/**
 * @file Measure.h
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
 * @brief Provides the base class for all measures implemented in OFIQ.
 * @author OFIQ development team
 */
#pragma once

#include "Configuration.h"
#include "ofiq_lib.h"
#include "Session.h"
#ifndef _WIN32
#    include <math.h>
#endif

/**
 * @brief Provides measures implemented in OFIQ.
 */
namespace OFIQ_LIB::modules::measures
{
    /**
     * @brief Parameters of the sigmoid function based quality
     * mapping.
     * @details A sigmoid-based quality mapping is the following function
     * \f[
     * Q(x)=h\cdot(a+s\cdot\mathrm{sigmoid}(x,x_0,w))
     * \f]
     * where
     * \f[
     * \mathrm{sigmoid}(x,x_0,w)=(1+\exp((x_0-x)/w)^{-1}.
     * \f]
     * \f$Q\f$ can be used to map a native quality score \f$x\f$ to a value
     * between 0 and 100. The other symbols denote parameters that 
     * can be configured using the struct.
     */
    struct SigmoidParameters
    {
        /**
         * @brief Default constructor.
         * @details After construction, all members are set to their default
         * values.
         */
        SigmoidParameters() { Reset(); }

        /**
         * @brief Scale factor.
         * @details The default value is 100.
         */
        double h;

        /**
         * @brief Constant shift
         * @details The default value is 0.
         */
        double a;

        /**
         * @brief Signed weight for sigmoid part
         * @details The default value is 1.
         */
        double s;

        /**
         * @brief Center point in sigmoid part.
         * @details The default value of 4 has been chosen arbitrarily 
         * and should specified when a mapping is configured.
         */
        double x0;

        /**
         * @brief Divisor in sigmoid part
         * @details The default value of 0.7 has been chosen arbitrarily
         * and should specified when a mapping is configured.
         */
        double w;

        /**
         * @brief Flag controlling if the compiler's native 
         * rounding function (<code>std::round</code>) is applied.
         * @details The default value is <code>true</code>.
         */
        bool round;

        /**
         * @brief Sets this quality mapping to a smaller-is-better variant.
         * @details If the parameters <code>a</code> is 0 and <code>s</code> is, 
         * then this quality mapping is in larger-is-better-semantics. For those
         * mappings, the method can be used to set the mapping to its smaller-is-
         * better counterpart by setting <code>a</code> to 1 and <code>s</code>
         * to -1. This is used by some measures to conveniently set a quality mapping.
         */
        void setInverse()
        {
            a = 1;
            s = -1;
        }

        /**
         * @brief Resets the members of the quality mapping to their default values.
         */
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

    /**
     * @brief Base class for measures implemented in OFIQ. 
     */
    class Measure
    {
    public:
        /**
         * @brief Constructor
         * @param configuration Configuration object from which measure-related configuration is read.
         * @param session Session object containing the original facial image and pre-processing results
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
         * OFIQImpl::performPreprocessing()\endlink method
         * @param measure Enum of the measure.
         */
        Measure(const Configuration& configuration, 
            const OFIQ_LIB::Session&,
            OFIQ::QualityMeasure measure)
            : configuration{configuration}, m_measure(measure)
        {
        }

        /**
         * @brief Abstract quality assessment function.
         * @details After quality assessment of the implemented measure, the method should invoke
         * \link OFIQ_LIB::modules::measures::Measure::SetQualityMeasure() SetQualityMeasure()\endlink
         * to insert the result of quality assessment in <code>session</code>.
         * @param session Session object containing the original facial image and pre-processing results
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
         * OFIQImpl::performPreprocessing()\endlink method.
         */
        virtual void Execute(OFIQ_LIB::Session& session) = 0;

        /**
         * @brief Destructor 
         */
        virtual ~Measure() = default;

        /**
         * @brief Returns the name of the measure.
         * @details Unless overwritten, the member \link OFIQ_LIB::modules::measures::Measure::m_measure 
         * m_measure\endlink is passed to the private \link OFIQ_LIB::modules::measures::Measure::GetMeasureName()
         * GetMeasureName()\endlink method and the result is returned.
         * @return std::string representation of the measures.
         */
        virtual std::string GetName() const;

        /**
         * @brief Returns an enum encoding the measure.
         * @return Enum encoding the measure.
         */
        virtual OFIQ::QualityMeasure GetQualityMeasure() const;

        /**
         * @brief Inserts the result of a quality assessment in the session object.
         * @details The method \link OFIQ_LIB::modules::measures::Measure::ExecuteScalarConversion(OFIQ::QualityMeasure,double) 
         * ExecuteScalarConversion()\endlink is invoked to map the native quality score to its
         * quality component value.
         * @param session Session object containing the original facial image and pre-processing results
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing() 
         * OFIQImpl::performPreprocessing()\endlink method.
         * @param measure Enum value specifying this measure.
         * @param rawValue Native quality score
         * @param code Value indicating whether the quality assessment was computed successfully or
         * otherwise (e.g., failureToAssess).
         */
        void SetQualityMeasure(OFIQ_LIB::Session& session, OFIQ::QualityMeasure measure, double rawValue, OFIQ::QualityMeasureReturnCode code);

    protected:
        /**
         * @brief Sigmoid function.
         * @param x Native quality score
         * @param x0 Non-zero center point
         * @param w Divisor
         * @return \f$(1+\exp((x0-x)/w))^{-1}\f$.
         */
        static double Sigmoid(double x, double x0, double w)
        {
            return 1.0 / (1 + exp((x0 - x) / w));
        }

        /**
         * @brief Reads sigmoid-function based quality mapping from the
         * configuration. 
         * @details The parameters are read from the configuration reference member
         * \link OFIQ_LIB::modules::measures::Measure::configuration \endlink. 
         * If a parameter is not configured,  its default value is chosen from the 
         * <code>defaultValues</code> argument.
         * @param measure Enum value encoding the measure for which the mapping
         * is configured.
         * @param defaultValues Parameters from which default values of
         * non-configured parameters are chosen.
         */
        void AddSigmoid(OFIQ::QualityMeasure measure, const SigmoidParameters& defaultValues);
     
        /**
         * @brief Reads sigmoid-function based quality mapping from the
         * configuration.
         * @details The parameters are read from the configuration reference member
         * \link OFIQ_LIB::modules::measures::Measure::configuration \endlink.
         * If a parameter is not configured,  its default value is chosen from the
         * <code>defaultValues</code> argument.
         * @param key Key/name of the measure of which mapping
         * is configured.
         * @param defaultValues Parameters from which default values of
         * non-configured parameters are chosen.
         */
        void AddSigmoid(const std::string& key, SigmoidParameters defaultValues);

        /**
         * @brief Maps a native quality score to a quality component value.
         * @param measure Enum value of the measure used to read parameters from a
         * private map member.
         * @param rawValue Native quality score.
         * @return Quality component value.
         */
        double ExecuteScalarConversion(OFIQ::QualityMeasure measure, double rawValue);

        /**
         * @brief Maps a native quality score to a quality component value.
         * @param key Key/name of the measure used to read parameters from a
         * private map member.
         * @param rawValue Native quality score.
         * @return Quality component value.
         */
        double ExecuteScalarConversion(const std::string& key, double rawValue);

        /**
         * @brief Reference to the configuration with which the measure constructor
         * has been invoked.
         */
        const Configuration& configuration;

    private:
        /**
         * @brief Applies a sigmoid-based quality mapping to a native quality
         * score and outputs the result.
         * @details Before output, the result is checked if it is below 0 or above
         * 100. If it is below 0, then 0 is returned. If it is above 100, then 100
         * is returned.
         * @param rawValue Native quality score.
         * @param par Parameters of the sigmoid-based quality mapping.
         * @return The mapped quality value.
         */
        static double ScalarConversion(double rawValue, const SigmoidParameters& par)
        {
            double scalarScore = par.h * (par.a + par.s * Sigmoid(rawValue, par.x0, par.w));
            if (par.round)
                scalarScore = round(scalarScore);
            if (scalarScore < 0.0)
            {
                scalarScore = 0.0;
            }
            else if (scalarScore > 100.0)
            {
                scalarScore = 100.0;
            }
            return scalarScore;
        }

        /**
         * @brief Used to map the measure name to the sigmoid-based
         * quality mapping function.
         */
        std::map<std::string, SigmoidParameters, std::less<>> sigmoidMap;
        
        /**
         * @brief Returns the name of the specified measure.
         * @param measure Enum value of a measure.
         * @return std::string representation of the requested measure.
         */
        static std::string GetMeasureName(OFIQ::QualityMeasure measure);

        /**
         * @brief Expands the raw key of a measure to the key accessing
         * its Sigmoid-based quality mapping.
         * @param rawKey representation of the measure (e.g., "BackgroundUniformity").
         * @return std::string representation of the key accessing the quality mapping
         * function configuration (e.g., "params.measures.BackgroundUniformity.Sigmoid").
         */
        static std::string ExpandKey(std::string_view rawKey);

        /**
         * @brief Value encoding the measure type.
         * @details The value is set to \link OFIQ::QualityMeasure::End QualityMeasure::NotSet\endlink
         * by default which effectively corresponds to a non-specified measure.
         */
        OFIQ::QualityMeasure m_measure = OFIQ::QualityMeasure::NotSet;
    };
}
