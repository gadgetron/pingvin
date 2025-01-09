#pragma once

#include "pipeline.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"
#include "gadgets/mri_core/RemoveROOversamplingGadget.h"
#include "gadgets/mri_core/AcquisitionAccumulateTriggerGadget.h"
#include "gadgets/mri_core/BucketToBufferGadget.h"
#include "gadgets/mri_core/SimpleReconGadget.h"
#include "gadgets/mri_core/ImageArraySplitGadget.h"
#include "gadgets/mri_core/ExtractGadget.h"

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

        // this->append<Gadgetron::NoiseAdjustGadget>();
        this->append<Gadgetron::RemoveROOversamplingGadget>();
        this->append<Gadgetron::AcquisitionAccumulateTriggerGadget>();
        this->append<Gadgetron::BucketToBufferGadget>();
        this->append<Gadgetron::SimpleReconGadget>();
        this->append<Gadgetron::ImageArraySplitGadget>();
        this->append<Gadgetron::ExtractGadget>();
    }
};

} // namespace pingvin