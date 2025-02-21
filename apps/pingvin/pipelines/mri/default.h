#pragma once

#include "Pipeline.h"

#include "MRSource.h"
#include "MRSink.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"
#include "gadgets/mri_core/PCACoilGadget.h"
#include "gadgets/mri_core/CoilReductionGadget.h"
#include "gadgets/mri_core/RemoveROOversamplingGadget.h"
#include "gadgets/mri_core/AcquisitionAccumulateTriggerGadget.h"
#include "gadgets/mri_core/BucketToBufferGadget.h"
#include "gadgets/mri_core/SimpleReconGadget.h"
#include "gadgets/mri_core/ImageArraySplitGadget.h"
#include "gadgets/mri_core/ExtractGadget.h"

namespace Pingvin {

  using namespace Gadgetron;

static auto default_mr = PipelineBuilder<Gadgetron::Core::MRContext>("default", "Basic Cartesian Reconstruction")
        .withSource<MRSource>()
        .withSink<MRSink>()
        .withNode<RemoveROOversamplingGadget>("ros")
        .withNode<AcquisitionAccumulateTriggerGadget>("acctrig")
        .withNode<BucketToBufferGadget>("buffer")
        .withNode<SimpleReconGadget>("recon")
        .withNode<ImageArraySplitGadget>("image-split")
        .withNode<ExtractGadget>("extract")
        ;

static auto default_mr_optimized = PipelineBuilder<Gadgetron::Core::MRContext>("default-optimized", "Basic Cartesian Reconstruction")
        .withSource<MRSource>()
        .withSink<MRSink>()
        .withNode<NoiseAdjustGadget>("noise-adjust")
        .withNode<PCACoilGadget>("pca")
        .withNode<CoilReductionGadget>("coil-reduction")
        .withNode<RemoveROOversamplingGadget>("ros")
        .withNode<AcquisitionAccumulateTriggerGadget>("acctrig")
        .withNode<BucketToBufferGadget>("buffer")
        .withNode<SimpleReconGadget>("recon")
        .withNode<ImageArraySplitGadget>("image-split")
        .withNode<ExtractGadget>("extract")
        ;

} // namespace pingvin