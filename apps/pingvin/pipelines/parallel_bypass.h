#pragma once

#include "Pipeline.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"
#include "gadgets/mri_core/PCACoilGadget.h"
#include "gadgets/mri_core/CoilReductionGadget.h"
#include "gadgets/mri_core/RemoveROOversamplingGadget.h"
#include "gadgets/mri_core/AcquisitionAccumulateTriggerGadget.h"
#include "gadgets/mri_core/BucketToBufferGadget.h"
#include "gadgets/mri_core/SimpleReconGadget.h"
#include "gadgets/mri_core/ImageArraySplitGadget.h"
#include "gadgets/mri_core/ExtractGadget.h"
#include "core/parallel/Fanout.h"
#include "gadgets/examples/ImageInverter.h"
#include "gadgets/examples/ImageLayerer.h"

namespace Pingvin {

  using namespace Gadgetron;

static auto example_parallel_bypass = PipelineBuilder<Gadgetron::Core::MrdContext>("parallel-bypass", "Basic Parallel Bypass Example")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<NoiseAdjustGadget>("noise")
        .withNode<RemoveROOversamplingGadget>("ros")
        .withNode<AcquisitionAccumulateTriggerGadget>("acctrig")
        .withNode<BucketToBufferGadget>("buffer")
        .withNode<SimpleReconGadget>("recon")
        .withNode<ImageArraySplitGadget>("image-split")
        .withNode<ExtractGadget>("extract")
        .withBranch<Core::Parallel::ImageFanout>("fanout")
            .withStream("unchanged")
            .withStream("inverted")
                .withNode<Examples::ImageInverter>("invert")
            .withMerge<Examples::ImageLayerer>("layer")
        ;

} // namespace pingvin