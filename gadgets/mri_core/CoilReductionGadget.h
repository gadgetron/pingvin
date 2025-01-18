/**
    \brief  Reduces number of coils in an acquisition based on a mask or threshold count
    \test   Tested by: simple_gre_3d.cfg, gpu_spiral_realtime_deblurring.cfg, gpu_fixed_radial_mode1_realtime.cfg, and others
*/

#pragma once

#include "Node.h"
#include "hoNDArray.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

namespace Gadgetron{
  class CoilReductionGadget : public Core::MRChannelGadget<mrd::Acquisition>
    {
      public:
        struct Parameters : public Core::NodeParameters {
          std::string coil_mask = "";
          unsigned int coils_out = 128;

          Parameters(const std::string& prefix) : NodeParameters(prefix, "Coil Reduction Options") {
            register_parameter("coil-mask", &coil_mask, "String mask of zeros and ones, e.g. 000111000 indicating which coils to keep");
            register_parameter("coils-out", &coils_out, "Number of coils to keep, coils with higher indices will be discarded");
          }

        };

        CoilReductionGadget(const Core::MrdContext& context, const Parameters& params);
        ~CoilReductionGadget() override = default;

        void process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& output) override;

      protected:
        const Parameters parameters_;

        std::vector<unsigned short> coil_mask_;
        unsigned int coils_in_;
        unsigned int coils_out_;
    };
}
