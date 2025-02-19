#pragma once

#include "Pipeline.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"

namespace Pingvin {

  using namespace Gadgetron;

  static auto noise_dependency = PipelineBuilder<Gadgetron::Core::MrdContext>("noise", "Compute noise covariance for measurement dependency")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<NoiseAdjustGadget>("noise")
        ;

} // namespace pingvin