#pragma once

#include "Node.h"

#include "MRContext.h"


namespace Gadgetron::Core {

template <class... TYPELIST> class MRChannelGadget : public ChannelGadget<TYPELIST...> {
public:
    using ChannelGadget<TYPELIST...>::ChannelGadget;

    MRChannelGadget(const MRContext& context, const NodeParameters& parameters) {}
};

} // namespace Gadgetron::Core
