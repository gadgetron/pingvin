#pragma once

#include "Node.h"
#include "hoNDArray.h"
#include "hoArmadillo.h"

#include <complex>

namespace Gadgetron{

  class EPIPackNavigatorGadget : public Core::ChannelGadget<mrd::Acquisition> {
    public:
      EPIPackNavigatorGadget(const Core::Context& context, const Core::GadgetProperties& props);

    protected:
      void process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out) override;

      // epi parameters
      int numNavigators_;
  };

}