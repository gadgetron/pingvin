#pragma once

#include "Pipeline.h"

#include "MRSource.h"
#include "MRSink.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"

namespace Pingvin {

  using namespace Gadgetron;

  static auto noise_dependency = PipelineBuilder<Gadgetron::Core::MRContext>("noise", "Compute noise covariance for measurement dependency")
        .withSource<MRSource>()
        .withSink<MRSink>()
        .withNode<NoiseAdjustGadget>("noise")
        ;

} // namespace pingvin