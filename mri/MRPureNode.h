#pragma once

#include "PureGadget.h"

#include "MRContext.h"

namespace Gadgetron::Core {

template <class RETURN, class INPUT>
class MRPureGadget : public PureGadget<RETURN, INPUT> {
public:
    MRPureGadget(const MRContext& context, const NodeParameters& parameters) {}
};

}