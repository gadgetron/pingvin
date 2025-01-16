/**
    \brief  Splits an ImageArray and outputs separate images
    \test   simple_gre_3d.cfg, distributed_simple_gre.cfg, and others
*/

#pragma once

#include "Node.h"
#include "hoNDArray.h"
#include "hoNDArray_math.h"

namespace Gadgetron{

  using ImageOrImageArray = std::variant<mrd::AnyImage, mrd::ImageArray>;

  class ImageArraySplitGadget : public Core::MRChannelGadget<ImageOrImageArray>
    {
      public:
        ImageArraySplitGadget(const Core::MrdContext& context, const Core::NodeParameters& params)
          : Core::MRChannelGadget<ImageOrImageArray>(context, params)
        { }

        void process(Core::InputChannel<ImageOrImageArray>& input, Core::OutputChannel& output) override;
    };
}
