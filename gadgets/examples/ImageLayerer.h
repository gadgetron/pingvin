#pragma once

#include "Node.h"
#include "parallel/Merge.h"

namespace Gadgetron::Examples {

    class ImageLayerer : public Core::Parallel::MRMerge {
    public:
        ImageLayerer(const Core::MrdContext &context, const Core::NodeParameters &params)
            : MRMerge(context, params)
        {}

        void process(std::map<std::string, Core::GenericInputChannel>, Core::OutputChannel) override;
    };
}
