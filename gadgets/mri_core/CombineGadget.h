/**
    \brief  Combines coils and outputs magnitude images
    \test   EPI_2d.cfg
*/

#pragma once

#include "PureGadget.h"
#include "hoNDArray_math.h"

namespace Gadgetron{
    class CombineGadget : public Core::MRPureGadget<mrd::AnyImage, mrd::AnyImage> {
    public:
        using MRPureGadget::MRPureGadget;

        mrd::AnyImage process_function(mrd::AnyImage image) const override;
    };
}
