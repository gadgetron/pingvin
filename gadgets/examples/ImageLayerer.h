#pragma once

#include "MRParallel.h"

namespace Gadgetron::Examples {

    class ImageLayerer : public Core::Parallel::MRMerge {
    public:
        ImageLayerer(const Core::MRContext &context, const Core::NodeParameters &params)
            : MRMerge(context, params)
        {}

        void process(std::map<std::string, Core::GenericInputChannel>, Core::OutputChannel) override;
    };
}
