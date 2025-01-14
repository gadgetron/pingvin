/**
    \brief  Performs basic FFT reconstruction on mrd::ReconData and passes along as mrd::ReconData
    \test   Tested by: simple_gre.cfg, simple_gre_python_image_array_recon.cfg, and others
*/

#pragma once

#include "Node.h"
#include "hoNDArray.h"

#include "hoNDArray_math.h"
#include "hoNDFFT.h"
#include <complex>

namespace Gadgetron {

    class SimpleReconGadget : public Core::ChannelGadget<mrd::ReconData> {
    public:
        using Core::ChannelGadget<mrd::ReconData>::ChannelGadget;
        SimpleReconGadget() : ChannelGadget("simple_recon") {}

        void process(Core::InputChannel<mrd::ReconData>& input, Core::OutputChannel& out) override;
    };
}