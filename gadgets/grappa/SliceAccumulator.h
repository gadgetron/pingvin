#pragma once

#include <vector>

#include "Node.h"

namespace Gadgetron::Grappa {

    using Slice = std::vector<mrd::Acquisition>;

    class SliceAccumulator : public Core::MRChannelGadget<mrd::Acquisition> {
    public:
        using Core::MRChannelGadget<mrd::Acquisition>::MRChannelGadget;

        void process(Core::InputChannel<mrd::Acquisition> &in, Core::OutputChannel &out) override;
    };
}

