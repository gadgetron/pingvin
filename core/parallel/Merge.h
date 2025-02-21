#pragma once

#include <map>

#include "Channel.h"
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
}