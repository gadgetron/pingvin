#pragma once

#include "PureGadget.h"

namespace Gadgetron::Examples {
    class ImageInverter : public Core::MRPureGadget<mrd::AnyImage, mrd::AnyImage> {
    public:
      using MRPureGadget::MRPureGadget;
      mrd::AnyImage process_function(mrd::AnyImage image) const override;
    };
}
