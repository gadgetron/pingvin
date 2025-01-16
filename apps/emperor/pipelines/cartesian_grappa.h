#pragma once

#include "pipeline.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"
#include "gadgets/mri_core/AsymmetricEchoAdjustROGadget.h"
#include "gadgets/mri_core/RemoveROOversamplingGadget.h"
#include "gadgets/mri_core/AcquisitionAccumulateTriggerGadget.h"
#include "gadgets/mri_core/BucketToBufferGadget.h"
#include "gadgets/mri_core/ImageArraySplitGadget.h"
#include "gadgets/mri_core/ComplexToFloatGadget.h"
#include "gadgets/mri_core/FloatToFixPointGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconCartesianReferencePrepGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconEigenChannelGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconCartesianGrappaGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconPartialFourierHandlingFilterGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconKSpaceFilteringGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconFieldOfViewAdjustmentGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconImageArrayScalingGadget.h"

namespace pingvin {

static auto cartesian_grappa = Pipeline::Builder<MrdContext>("cartesian-grappa", "Cartesian Grappa Recon")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<NoiseAdjustGadget>("noise")
        .withNode<AsymmetricEchoAdjustROGadget>("asymmetric-echo")
        .withNode<RemoveROOversamplingGadget>("ros")
        .withNode<AcquisitionAccumulateTriggerGadget>("acquisition-accumulate")
        .withNode<BucketToBufferGadget>("bucket-to-buffer")
        .withNode<GenericReconCartesianReferencePrepGadget>("reference-prep")
        .withNode<GenericReconEigenChannelGadget>("eigen-channel")
        .withNode<GenericReconCartesianGrappaGadget>("grappa")
        .withNode<GenericReconPartialFourierHandlingFilterGadget>("pf-handling")
        .withNode<GenericReconKSpaceFilteringGadget>("kspace-filtering")
        .withNode<GenericReconFieldOfViewAdjustmentGadget>("fov-adjustment")
        .withNode<GenericReconImageArrayScalingGadget>("image-array-scaling")
        .withNode<ImageArraySplitGadget>("image-split")
        .withNode<ComplexToFloatGadget>("complex-to-float")
        .withNode<FloatToUShortGadget>("float-to-ushort");

} // namespace pingvin