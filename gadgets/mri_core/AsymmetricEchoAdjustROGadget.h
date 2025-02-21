/**
    \brief  Re-aligns the readout data with the center of the echo at the center of the incoming array (if not a noise scan and partial fourier along readout is detected)
    \author Original: Hui Xue
    \author ChannelGadget Conversion: Andrew Dupuis
    \test   Tested by: gpu_grappa_simple.cfg, cpu_grappa_simple.cfg, generic_cartesian_cine_denoise.cfg, and others
*/

#pragma once

#include "MRNode.h"
#include "hoNDArray.h"

namespace Gadgetron{
  class AsymmetricEchoAdjustROGadget : public Core::MRChannelGadget<mrd::Acquisition>
    {
      public:
        AsymmetricEchoAdjustROGadget(const Core::MRContext& context, const Parameters& params);

        void process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& output) override;

      protected:
        std::vector<unsigned int> maxRO_;
    };
}