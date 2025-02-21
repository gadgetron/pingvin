#pragma once

#include "Channel.h"

namespace Pingvin {

struct ISource {
    virtual ~ISource() = default;
    virtual void consume_input(Gadgetron::Core::ChannelPair& input_channel) = 0;
};

template <typename CTX>
struct Source : public ISource {
    virtual void initContext(CTX&) = 0;
};

}