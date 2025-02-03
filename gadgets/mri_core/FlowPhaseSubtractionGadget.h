#pragma once

#include "Node.h"
#include "hoNDArray.h"

#include <complex>

namespace Gadgetron{

    class FlowPhaseSubtractionGadget : public Core::MRChannelGadget<mrd::Image<std::complex<float>>>
    {

    public:
        FlowPhaseSubtractionGadget(const Core::MrdContext& context, const Core::NodeParameters& params);
        ~FlowPhaseSubtractionGadget() override = default;

        void process(Core::InputChannel<mrd::Image<std::complex<float>>>& in, Core::OutputChannel& out) override;

    protected:
        uint32_t sets_;
    };
}

