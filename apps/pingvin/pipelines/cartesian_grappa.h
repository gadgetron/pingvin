#pragma once

#include "pipeline.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"
#include "gadgets/mri_core/AsymmetricEchoAdjustROGadget.h"
#include "gadgets/mri_core/RemoveROOversamplingGadget.h"
#include "gadgets/mri_core/AcquisitionAccumulateTriggerGadget.h"
#include "gadgets/mri_core/BucketToBufferGadget.h"
#include "gadgets/mri_core/ImageArraySplitGadget.h"
#include "gadgets/mri_core/ComplexToFloatGadget.h"
#include "gadgets/mri_core/FloatToFixedPointGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconCartesianReferencePrepGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconEigenChannelGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconCartesianGrappaGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconPartialFourierHandlingFilterGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconKSpaceFilteringGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconFieldOfViewAdjustmentGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconImageArrayScalingGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconNoiseStdMapComputingGadget.h"

namespace pingvin {

static auto cartesian_grappa = PipelineBuilder<MrdContext>("cartesian-grappa", "Cartesian Grappa Recon")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<NoiseAdjustGadget>("noise")
        .withNode<AsymmetricEchoAdjustROGadget>("echo-adjust")
        .withNode<RemoveROOversamplingGadget>("ros")
        .withNode<AcquisitionAccumulateTriggerGadget>("acctrig")
        .withNode<BucketToBufferGadget>("buffer")
        .withNode<GenericReconCartesianReferencePrepGadget>("refprep")
        .withNode<GenericReconEigenChannelGadget>("coilcomp")
        .withNode<GenericReconCartesianGrappaGadget>("grappa")
        .withNode<GenericReconPartialFourierHandlingFilterGadget>("pf")
        .withNode<GenericReconKSpaceFilteringGadget>("kspace-filter")
        .withNode<GenericReconFieldOfViewAdjustmentGadget>("fov-adjust")
        .withNode<GenericReconImageArrayScalingGadget>("scale")
        .withNode<ImageArraySplitGadget>("split")
        .withNode<ComplexToFloatGadget>("complex-to-float")
        .withNode<FloatToFixedPointGadget>("convert")
        ;

static auto cartesian_grappa_snr = PipelineBuilder<MrdContext>("cartesian-grappa-snr", "Cartesian Grappa Recon with SNR")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<NoiseAdjustGadget>("noise")
        .withNode<AsymmetricEchoAdjustROGadget>("echo-adjust")
        .withNode<RemoveROOversamplingGadget>("ros")
        .withNode<AcquisitionAccumulateTriggerGadget>("acctrig")
        .withNode<BucketToBufferGadget>("buffer")
        .withNode<GenericReconCartesianReferencePrepGadget>("refprep")
        .withNode<GenericReconCartesianGrappaGadget>("grappa")
        .withNode<GenericReconPartialFourierHandlingFilterGadget>("pf")
        .withNode<GenericReconKSpaceFilteringGadget>("kspace-filter")
        .withNode<GenericReconFieldOfViewAdjustmentGadget>("fov-adjust")
        .withNode<GenericReconNoiseStdMapComputingGadget>("std-map")
        .withNode<GenericReconImageArrayScalingGadget>("scale")
        .withNode<ImageArraySplitGadget>("split")
        .withNode<ComplexToFloatGadget>("complex-to-float")
        .withNode<FloatToFixedPointGadget>("convert")
        ;

} // namespace pingvin