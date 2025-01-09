#pragma once
#include "Node.h"
#include "hoNDArray.h"

#include <bitset>
#include <complex>

namespace Gadgetron {

class ExtractGadget : public Core::ChannelGadget<mrd::Image<std::complex<float>>> {
  public:
    using Core::ChannelGadget<mrd::Image<std::complex<float>>>::ChannelGadget;
    ExtractGadget(const Core::Context& context, const Core::GadgetProperties& props);

    void process(Core::InputChannel<mrd::Image<std::complex<float>>>& in, Core::OutputChannel& out) override;

    virtual void install_cli(po::options_description& options) override {
        options.add_options()
            ("extract_magnitude", po::bool_switch(&extract_magnitude), "Extract absolute value")
            ("extract_real", po::bool_switch(&extract_real), "Extract real components")
            ("extract_imag", po::bool_switch(&extract_imag), "Extract imaginary component")
            ("extract_phase", po::bool_switch(&extract_phase), "Extract phase")
            ("real_imag_offset", po::value<float>(&real_imag_offset)->default_value(0.0f), "Offset to add to real and imag images");
    }

  protected:
    NODE_PROPERTY(extract_mask, std::bitset<4>, "(DEPRECATED) Extract mask, bitmask MAG=1, REAL=2, IMAG=4, PHASE=8", 0);
    NODE_PROPERTY(extract_magnitude, bool, "Extract absolute value", true);
    NODE_PROPERTY(extract_real, bool, "Extract real components", false);
    NODE_PROPERTY(extract_imag, bool, "Extract imaginary component", false);
    NODE_PROPERTY(extract_phase, bool, "Extract phase", false);
    NODE_PROPERTY(real_imag_offset, float, "Offset to add to real and imag images", 0.0f);

    virtual void initialize_(const Core::Context& context) override {
        if (!extract_magnitude && !extract_real && !extract_imag && !extract_phase) {
            // throw std::runtime_error("No images selected for extraction");
            GADGET_THROW("No images selected for extraction");
        }

        if (extract_magnitude) image_types.insert(mrd::ImageType::kMagnitude);
        if (extract_real) image_types.insert(mrd::ImageType::kReal);
        if (extract_imag) image_types.insert(mrd::ImageType::kImag);
        if (extract_phase) image_types.insert(mrd::ImageType::kPhase);
    }

    std::set<mrd::ImageType> image_types;
};

}
