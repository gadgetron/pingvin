#pragma once

#include "pipeline.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"
#include "gadgets/mri_core/RemoveROOversamplingGadget.h"
#include "gadgets/mri_core/ExtractGadget.h"

namespace pingvin {

class GrappaCPU : public Pipeline {
  public:
    GrappaCPU() : Pipeline("grappa_cpu", "Grappa Reconstruction on CPU") { }

    void build(void) override {
        this->append<Gadgetron::NoiseAdjustGadget>();
        this->append<Gadgetron::PCACoilGadget>();
        this->append<Gadgetron::CoilReductionGadget>();
        this->append<Gadgetron::AsymmetricEchoAdjustROGadget>();
        this->append<Gadgetron::RemoveROOversamplingGadget>();
        this->append<Gadgetron::SliceAccumulator>();
        // Parallel
        //   Branch: AcquisitionFanout
        //   Stream
        //     ImageAccumulator
        //   Stream
        //     cpuWeightsCalculator
        //   Merge: Unmixing
        this->append<Gadgetron::ExtractGadget>();
        this->append<Gadgetron::AutoScaleGadget>();
        this->append<Gadgetron::FloatToUShortGadget>();
    }
};

} // namespace pingvin