#ifndef CutXGADGET_H
#define CutXGADGET_H

#include "Node.h"
#include "hoNDArray.h"

#include <complex>

namespace Gadgetron{

  class CutXGadget : public Core::ChannelGadget<mrd::Acquisition>
  {
    public:
      CutXGadget(const Core::Context& context, const Core::GadgetProperties& props);

    protected:
      NODE_PROPERTY(verbose_mode_, bool, "Verbose output", false);

      void process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out) override;

      size_t encodeNx_;
      float encodeFOV_;
      size_t reconNx_;
      float reconFOV_;

      size_t cutNx_;
  };
}
#endif //CutXGADGET_H
