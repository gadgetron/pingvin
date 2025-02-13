#pragma once

#include <map>
#include <memory>
#include <boost/dll.hpp>

#include "Channel.h"
#include "Context.h"
#include "Parameters.h"

namespace Gadgetron::Core::Parallel {

    class Merge {
    public:
        struct Parameters : NodeParameters {
            using NodeParameters::NodeParameters;
        };

        virtual ~Merge() = default;
        virtual void process(std::map<std::string, GenericInputChannel>, OutputChannel) = 0;
    };

    /** TODO: Move to MR-specific location! */
    class MRMerge : public Merge {
    public:
        using Merge::Merge;

        MRMerge(const MrdContext& context, const NodeParameters& parameters) {}
    };
}