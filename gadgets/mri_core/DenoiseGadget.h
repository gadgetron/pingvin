#pragma once

#include "Node.h"
#include "hoNDArray.h"

#include "PureGadget.h"
#include <string>

namespace Gadgetron {

    using DenoiseSupportedTypes =
        std::variant<mrd::Image<float>, mrd::Image<std::complex<float>>, mrd::ImageArray>;

    class DenoiseGadget : public Core::MRPureGadget<DenoiseSupportedTypes, DenoiseSupportedTypes> {

    public:
        struct Parameters : public Core::NodeParameters {
            float image_std = 1;
            int search_radius = 25;
            std::string denoiser = "non_local_bayes";

            Parameters(const std::string& prefix) : Core::NodeParameters(prefix, "Denoising Options")
            {
                register_parameter("image_std", &image_std, "Standard deviation of the noise in the produced image");
                register_parameter("search_radius", &search_radius, "Standard deviation of the noise in the produced image");
                register_parameter("denoiser", &denoiser, "Type of denoiser - non_local_means or non_local_bayes");
            }
        };

        DenoiseGadget(const Core::MrdContext& context, const Parameters& params)
            : Core::MRPureGadget<DenoiseSupportedTypes, DenoiseSupportedTypes>(context, params)
            , parameters_(params)
        {}

        DenoiseSupportedTypes process_function(DenoiseSupportedTypes input) const;

    protected:
        const Parameters parameters_;

        template <class T>
        mrd::Image<T> denoise(mrd::Image<T> image) const;
        mrd::ImageArray denoise(mrd::ImageArray image_array) const;

        template <class T> hoNDArray<T> denoise_function(const hoNDArray<T>&) const;
    };

}