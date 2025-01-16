#pragma once

#include "Node.h"
#include "hoNDArray.h"

#include <complex>

namespace Gadgetron{

    class FlowPhaseSubtractionGadget : public Core::ChannelGadget<mrd::Image<std::complex<float>>>
    {

    public:
        FlowPhaseSubtractionGadget(const Core::Context& context, const Core::GadgetProperties& props);

        ~FlowPhaseSubtractionGadget() override = default;

        void process(Core::InputChannel<mrd::Image<std::complex<float>>>& in, Core::OutputChannel& out) override;

    protected:
        uint32_t sets_;
    };
}

