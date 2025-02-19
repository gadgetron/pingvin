#pragma once

#include "Channel.h"

namespace Pingvin {

struct ISink {
    virtual ~ISink() = default;
    virtual void produce_output(Gadgetron::Core::ChannelPair& output_channel) = 0;
};

}