#pragma once

#include "pipeline.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"

namespace pingvin {

  using namespace Gadgetron;

  static auto noise_dependency = PipelineBuilder<MrdContext>("noise", "Compute noise covariance for measurement dependency")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<NoiseAdjustGadget>("noise")
        ;

} // namespace pingvin