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

        GenericReconNoiseStdMapComputingGadget(const Core::Context& context, const Core::GadgetProperties& properties);

        /// ------------------------------------------------------------------------------------
        /// parameters to control the reconstruction
        /// ------------------------------------------------------------------------------------

        /// ------------------------------------------------------------------------------------
        /// start N index to compute std map
        NODE_PROPERTY(start_N_for_std_map, int, "Start N index to compute std map", 5);

        // ------------------------------------------------------------------------------------

    protected:

        // --------------------------------------------------
        // functional functions
        // --------------------------------------------------

        // default interface function
        void process(Core::InputChannel<mrd::ImageArray>& in, Core::OutputChannel& out) override;

    };
}
