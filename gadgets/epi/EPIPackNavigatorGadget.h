#pragma once

#include "Node.h"
#include "hoNDArray.h"
#include "hoArmadillo.h"

#include <complex>

namespace Gadgetron{

  class EPIPackNavigatorGadget : public Core::MRChannelGadget<mrd::Acquisition> {
    public:
      EPIPackNavigatorGadget(const Core::MrdContext& context, const Core::NodeParameters& parameters);

    protected:
      void process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out) override;

      // epi parameters
      int numNavigators_;
  };

}