#pragma once

#include "Gadget.h"

namespace pingvin {

class RemoveOversamplingGadget : public Gadget {
  public:
    RemoveOversamplingGadget() : Gadget("Remove Oversampling", "Remove readout oversampling") {}

    virtual void process() override {
        std::cerr << "RemoveOversamplingGadget::process" << std::endl;
        std::cerr << "  skip: " << skip_ << std::endl;
    }

    void install_cli(po::options_description& desc) override {
        std::cerr << "RemoveOversamplingGadget::install_cli" << std::endl;

        desc.add_options()
            ("skip", po::value<bool>(&skip_), "Skip this gadget")
            ;
    }

private:
    bool skip_ = false;
};

} // namespace pingvin