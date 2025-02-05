#pragma once

#include <vector>

#include "ChannelReorderer.h"

#include "Node.h"

namespace Gadgetron::Grappa {

    using Slice = std::vector<AnnotatedAcquisition>;

    class SliceAccumulator : public Core::MRChannelGadget<AnnotatedAcquisition> {
    public:
        using Core::MRChannelGadget<AnnotatedAcquisition>::MRChannelGadget;

        void process(Core::InputChannel<AnnotatedAcquisition> &in, Core::OutputChannel &out) override;
    };
}

