#pragma once

#include "MRNode.h"
#include "hoNDArray.h"

#include <complex>

namespace Gadgetron{

  class FFTXGadget : public Core::MRChannelGadget<mrd::Acquisition> {
    public:
      using Core::MRChannelGadget<mrd::Acquisition>::MRChannelGadget;

    protected:
      void process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out) override;
  };

}