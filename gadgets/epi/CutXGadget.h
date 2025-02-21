#pragma once

#include "MRNode.h"
#include "hoNDArray.h"

#include <complex>

namespace Gadgetron{

  class CutXGadget : public Core::MRChannelGadget<mrd::Acquisition>
  {
    public:
      CutXGadget(const Core::MRContext& context, const Core::NodeParameters& parameters);

    protected:
      void process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out) override;

      size_t encodeNx_;
      float encodeFOV_;
      size_t reconNx_;
      float reconFOV_;

      size_t cutNx_;
  };

}