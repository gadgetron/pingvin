/*
 *       ComplexToFloatGadget.cpp
 *       Author: Hui Xue
 */

#include "ComplexToFloatGadget.h"
#include "hoNDArray_elemwise.h"

#include "log.h"


namespace Gadgetron {

ComplexToFloatGadget::ComplexToFloatGadget(const Core::MrdContext& context, const Parameters& params)
    : MRPureGadget(context, params)
{

    converters = { { mrd::ImageType::kMagnitude, [](const auto& image) { return abs(image); } },
                   { mrd::ImageType::kPhase,     [](const auto& image) { return argument(image); } },
                   { mrd::ImageType::kReal,      [](const auto& image) { return real(image); } },
                   { mrd::ImageType::kImag,      [](const auto& image) { return imag(image); } }};
};

mrd::Image<float> ComplexToFloatGadget::process_function(
    mrd::Image<std::complex<float>> input_image) const
{
    mrd::Image<float> out;
    out.head = input_image.head;
    out.meta = input_image.meta;

    hoNDArray<float> output_data;

    if (converters.count(input_image.head.image_type)) {
        out.data = converters.at(input_image.head.image_type)(input_image.data);
    }
    else {
        GDEBUG_STREAM("Image type not set; defaulting to magnitude image.");
        out.data = converters.at(mrd::ImageType::kMagnitude)(input_image.data);
        out.head.image_type = mrd::ImageType::kMagnitude;
    }

    return out;
}

GADGETRON_GADGET_EXPORT(ComplexToFloatGadget)
}