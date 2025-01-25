#pragma once

#include "Node.h"
#include "hoNDArray.h"

#include <complex>

namespace Gadgetron{

  class FFTXGadget : public Core::ChannelGadget<mrd::Acquisition> {
    public:
      FFTXGadget(const Core::Context& context, const Core::GadgetProperties& props)
        : ChannelGadget(context, props) {}

    protected:
      void process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out) override;
  };

}