/**
    \brief
    \test   Tested by: fs_csi.cfg
*/

#pragma once

#include "Node.h"
#include "hoNDArray.h"

namespace Gadgetron{
  class AccumulatorGadget : public Core::MRChannelGadget<mrd::Acquisition>
    {
      public:
        struct Parameters : public Core::NodeParameters
        {
            long long image_series = 1;
            Parameters(const std::string& prefix) : Core::NodeParameters(prefix, "Accumulator Gadget")
            {
                register_parameter("image_series", &image_series, "Image series number");
            }
        };

        AccumulatorGadget(const Core::MrdContext& context, const Parameters& params);
        void process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& output) override;

      protected:
        const Parameters parameters_;

        hoNDArray<std::complex<float>> buffer_;
        std::vector<size_t> dimensions_;
        float field_of_view_[3];
        size_t slices_;
        long long image_counter_;
    };
}