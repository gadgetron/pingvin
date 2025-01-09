#pragma once

#include "pipeline.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"

namespace pingvin {

class CartesianGrappa : public Pipeline {
  public:
    CartesianGrappa() : Pipeline("cartesian-grappa", "Cartesian Grappa Recon") { }

    void build(void) override {
        gadgets_.push_back(std::make_shared<Gadgetron::NoiseAdjustGadget>());
        // gadgets_.push_back(std::make_unique<RemoveOversamplingGadget>());
    }
};

} // namespace pingvin