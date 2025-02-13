
#include "DenoiseGadget.h"
#include "GadgetronTimer.h"
#include "non_local_bayes.h"
#include "non_local_means.h"
#include "mri_core_utility.h"

namespace Gadgetron {

    template <class T>
    hoNDArray<T> DenoiseGadget::denoise_function(const hoNDArray<T>& input) const {

        if (parameters_.denoiser == "non_local_bayes") {
            return Denoise::non_local_bayes(input, parameters_.image_std, parameters_.search_radius);
        } else if (parameters_.denoiser == "non_local_means") {
            return Denoise::non_local_means(input, parameters_.image_std, parameters_.search_radius);
        } else {
            throw std::invalid_argument(std::string("DenoiseGadget: Unknown denoiser type: ") + std::string(parameters_.denoiser));
        }
    }

    DenoiseSupportedTypes DenoiseGadget::process_function(DenoiseSupportedTypes input) const {
        return std::visit(
            [&,this](auto& image) { return DenoiseSupportedTypes(this->denoise(std::move(image))); }, input);
    }

    template <class T> mrd::Image<T> DenoiseGadget::denoise(mrd::Image<T> image) const {
        image.data = denoise_function(image.data);
        return std::move(image);
    }

    mrd::ImageArray DenoiseGadget::denoise(mrd::ImageArray image_array) const {
        image_array.data = denoise_function(image_array.data);
        return std::move(image_array);
    }

}