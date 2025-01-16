#pragma once

#include "pipeline.h"

#include "gadgets/mri_core/RemoveROOversamplingGadget.h"
#include "gadgets/mri_core/AcquisitionAccumulateTriggerGadget.h"
#include "gadgets/mri_core/BucketToBufferGadget.h"
#include "gadgets/mri_core/SimpleReconGadget.h"
#include "gadgets/mri_core/ImageArraySplitGadget.h"
#include "gadgets/mri_core/ExtractGadget.h"

namespace pingvin {

  using namespace Gadgetron;

static auto default_mr = Pipeline::Builder<MrdContext>("default", "Basic Cartesian Reconstruction")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<RemoveROOversamplingGadget>("ros")
        .withNode<AcquisitionAccumulateTriggerGadget>("accumulate")
        .withNode<BucketToBufferGadget>("buffer")
        .withNode<SimpleReconGadget>("recon")
        .withNode<ImageArraySplitGadget>("image-split")
        .withNode<ExtractGadget>("extract")
        ;

} // namespace pingvin