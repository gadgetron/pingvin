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
#include "gadgets/cmr/CmrCartesianKSpaceBinningCineGadget.h"
#include "gadgets/cmr/CmrParametricT1SRMappingGadget.h"

namespace pingvin {

static auto cmr_cine_binning = PipelineBuilder<MrdContext>("cmr-cine-binning", "CMR cine binning 2slices")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<NoiseAdjustGadget>("noise")
        .withNode<AsymmetricEchoAdjustROGadget>("asymmetric-echo")
        .withNode<RemoveROOversamplingGadget>("ros")
        .withNode<AcquisitionAccumulateTriggerGadget>("acctrig")
        .withNode<BucketToBufferGadget>("buffer")
        .withNode<GenericReconCartesianReferencePrepGadget>("refprep")
        .withNode<GenericReconEigenChannelGadget>("coilcomp")
        .withNode<CmrCartesianKSpaceBinningCineGadget>("cmr")
        .withNode<GenericReconPartialFourierHandlingPOCSGadget>("pf")
        .withNode<GenericReconKSpaceFilteringGadget>("kspace-filter")
        .withNode<GenericReconFieldOfViewAdjustmentGadget>("fov-adjust")
        .withNode<GenericReconImageArrayScalingGadget>("scale")
        .withNode<ImageArraySplitGadget>("split")
        .withNode<ComplexToFloatGadget>("complex-to-float")
        .withNode<FloatToFixedPointGadget>("convert")
        ;

static auto cmr_mapping_t1_sr = PipelineBuilder<MrdContext>("cmr-mapping-t1-sr", "CMR 2DT T1 mapping SASHA")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<NoiseAdjustGadget>("noise")
        .withNode<AsymmetricEchoAdjustROGadget>("asymmetric-echo")
        .withNode<RemoveROOversamplingGadget>("ros")
        .withNode<AcquisitionAccumulateTriggerGadget>("acctrig")
        .withNode<BucketToBufferGadget>("buffer")
        .withNode<GenericReconCartesianReferencePrepGadget>("refprep")
        .withNode<GenericReconEigenChannelGadget>("coilcomp")
        .withNode<GenericReconCartesianGrappaGadget>("grappa")
        .withNode<GenericReconPartialFourierHandlingPOCSGadget>("pf")
        .withNode<GenericReconKSpaceFilteringGadget>("kspace-filter")
        .withNode<GenericReconFieldOfViewAdjustmentGadget>("fov-adjust")
        .withNode<GenericReconImageArrayScalingGadget>("scale")
        .withNode<CmrParametricT1SRMappingGadget>("sasha")
        .withNode<ImageArraySplitGadget>("split")
        .withNode<ComplexToFloatGadget>("complex-to-float")
        .withNode<FloatToFixedPointGadget>("convert")
        ;

} // namespace pingvin