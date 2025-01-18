/** \file   AugmentImageMetadataGadget.h
    \brief  This Gadget augments the image metadata headers by adding some fields that can be required by clients
    \author Michael Hansen
*/

#pragma once
#include "hoNDArray.h"

#include "PureGadget.h"

namespace Gadgetron
{
class AugmentImageMetadataGadget: public Core::MRPureGadget<mrd::Image<std::complex<float>>,mrd::Image<std::complex<float>>>
{
public:
    using Core::MRPureGadget<mrd::Image<std::complex<float>>,mrd::Image<std::complex<float>>>::MRPureGadget;

    mrd::Image<std::complex<float>> process_function(mrd::Image<std::complex<float>> args) const override;
};
}
