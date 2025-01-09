#pragma once

#include "Gadget.h"

namespace pingvin {

class NoiseAdjustGadget : public Gadget {
  public:
    NoiseAdjustGadget() : Gadget("Noise Adjust", "Noise covariance estimation") {}

    virtual void process() override {
        std::cerr << "NoiseAdjustGadget::process" << std::endl;
        std::cerr << "  noise_covariance_out: " << noise_covariance_out_ << std::endl;
        std::cerr << "  pass_nonconformant_data: " << pass_nonconformant_data_ << std::endl;
    }

    virtual void install_cli(po::options_description& desc) override {
        std::cerr << "NoiseAdjustGadget::install_cli" << std::endl;

        desc.add_options()
            ("noise_covariance_out", po::value<std::string>(&noise_covariance_out_), "Noise covariance output file")
            ("pass_nonconformant_data", po::value<bool>(&pass_nonconformant_data_), "Pass data that does not conform")
            ;
    }

private:
    std::string noise_covariance_out_;
    bool pass_nonconformant_data_ = true;
};

} // namespace pingvin