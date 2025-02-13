#pragma once

#include <Context.h>
#include <Channel.h>

#include "Merge.h"

namespace Gadgetron::Core::Parallel {

    class UnorderedMerge : public Merge {
    public:
        using Merge::Merge;
        void process(std::map<std::string, GenericInputChannel>, OutputChannel) override;
    };
}