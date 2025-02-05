#pragma once

#include "SliceAccumulator.h"
#include "common/AcquisitionBuffer.h"

#include "Channel.h"
#include "Node.h"

namespace Gadgetron::Grappa {

    class ImageAccumulator : public Core::MRChannelGadget<Slice> {
    public:
        ImageAccumulator(const Core::MrdContext &context, const Core::NodeParameters& params)
            : Core::MRChannelGadget<Slice>(context, params)
            , buffer(context.header)
        {}

        void process(Core::InputChannel<Slice> &in, Core::OutputChannel &out) override;

    private:
        AcquisitionBuffer buffer;
    };
}
