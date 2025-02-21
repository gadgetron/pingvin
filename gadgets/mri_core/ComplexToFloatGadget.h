/** \file   ComplexToFloatGadget.h
    \brief  This Gadget converts complex float values to float format.
    \author Hui Xue
*/

#pragma once

#include "MRPureNode.h"

namespace Gadgetron
{
class ComplexToFloatGadget: public Core::MRPureGadget<mrd::Image<float>,mrd::Image<std::complex<float>>>
    {
    public:
        ComplexToFloatGadget(const Core::MRContext&, const Parameters&);

        mrd::Image<float> process_function(mrd::Image<std::complex<float>> args) const override;
    private:
        std::map<mrd::ImageType, std::function<mrd::ImageData<float>(const mrd::ImageData<std::complex<float>>&)>> converters;
};
}
