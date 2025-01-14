#pragma once

#include "pipeline.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"

namespace pingvin {

class NoiseDependency: public Pipeline {
  public:
    NoiseDependency() : Pipeline("noise", "Compute noise covariance for measurement dependency") { }

    void build(void) override {
        gadgets_.push_back(std::make_shared<Gadgetron::NoiseAdjustGadget>());
    }
};

} // namespace pingvin