#pragma once

#include "pipeline.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"
#include "gadgets/mri_core/RemoveROOversamplingGadget.h"

namespace pingvin {

class Default : public Pipeline {
  public:
    Default() : Pipeline("default", "Basic Cartesian Reconstruction") { }

    void build(void) override {
        // gadgets_.push_back(std::make_shared<Gadgetron::NoiseAdjustGadget>());
        // gadgets_.push_back(std::make_shared<Gadgetron::RemoveROOversamplingGadget>());

        // gadgets_ = Builder()
        //     .append<Gadgetron::NoiseAdjustGadget>()
        //     .append<Gadgetron::RemoveROOversamplingGadget>()
        //     .build();

        this->append<Gadgetron::NoiseAdjustGadget>();
        this->append<Gadgetron::RemoveROOversamplingGadget>();
    }
};

} // namespace pingvin