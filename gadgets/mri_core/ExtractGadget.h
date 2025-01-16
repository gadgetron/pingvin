#pragma once
#include "Node.h"
#include "hoNDArray.h"

#include <bitset>
#include <complex>

namespace Gadgetron {

class ExtractGadget : public Core::MRChannelGadget<mrd::Image<std::complex<float>>> {
  public:
    struct Parameters : public Core::NodeParameters {
        using NodeParameters::NodeParameters;
        Parameters(const std::string& prefix) : NodeParameters(prefix, "Extract Options") {
            register_parameter("mask", &extract_mask, "(DEPRECATED) Extract mask, bitmask MAG=1, REAL=2, IMAG=4, PHASE=8");
            register_flag("magnitude", &extract_magnitude, "Extract absolute value");
            register_flag("real", &extract_real, "Extract real components");
            register_flag("imag", &extract_imag, "Extract imaginary component");
            register_flag("phase", &extract_phase, "Extract phase");
            register_parameter("real-imag-offset", &real_imag_offset, "Offset to add to real and imag images");
        }
        std::bitset<4> extract_mask = 0;
        bool extract_magnitude = false;
        bool extract_real = false;
        bool extract_imag = false;
        bool extract_phase = false;
        float real_imag_offset = 0.0f;
    };

    ExtractGadget(const Core::MrdContext& context, const Parameters& params);

    void process(Core::InputChannel<mrd::Image<std::complex<float>>>& in, Core::OutputChannel& out) override;

  protected:
    const Parameters parameters_;

    std::set<mrd::ImageType> image_types;
};

}
