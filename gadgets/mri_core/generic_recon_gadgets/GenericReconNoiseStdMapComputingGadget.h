/** \file   GenericReconNoiseStdMapComputingGadget.h
    \brief  This is the class gadget to compute standard deviation map, working on the mrd::ImageArray.

            This class is a part of general cartesian recon chain. It computes the std map on incoming SNR images.

\author     Hui Xue
*/

#pragma once

#include "GenericReconBase.h"

namespace Gadgetron {

    class GenericReconNoiseStdMapComputingGadget : public GenericReconImageArrayBase
    {
    public:
        typedef float real_value_type;
        typedef std::complex<real_value_type> ValueType;
        typedef ValueType T;

        typedef GenericReconImageArrayBase BaseClass;

        struct Parameters : public BaseClass::Parameters {
            Parameters(const std::string& prefix) : BaseClass::Parameters(prefix, "Computed Noise Std Map")
            {
                register_parameter("start_N_for_std_map", &start_N_for_std_map, "Start N index to compute std map");
            }

            int start_N_for_std_map = 5;
        };

        GenericReconNoiseStdMapComputingGadget(const Core::MRContext& context, const Parameters& params);
    protected:
        const Parameters params_;

        // --------------------------------------------------
        // functional functions
        // --------------------------------------------------

        // default interface function
        void process(Core::InputChannel<mrd::ImageArray>& in, Core::OutputChannel& out) override;

    };
}
