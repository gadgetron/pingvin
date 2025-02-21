#pragma once

#include "Pipeline.h"

#include "MRSource.h"
#include "MRSink.h"

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
#include "gadgets/cmr/CmrRealTimeLAXCineAIAnalysisGadget.h"

namespace Pingvin {

static auto cmr_cine_binning = PipelineBuilder<Gadgetron::Core::MRContext>("cmr-cine-binning", "CMR cine binning 2slices")
        .withSource<MRSource>()
        .withSink<MRSink>()
        .withNode<NoiseAdjustGadget>("noise")
        .withNode<AsymmetricEchoAdjustROGadget>("asymmetric-echo")
        .withNode<RemoveROOversamplingGadget>("ros")
        .withNode<AcquisitionAccumulateTriggerGadget>("acctrig")
        .withNode<BucketToBufferGadget>("buffer")
        .withNode<GenericReconCartesianReferencePrepGadget>("refprep")
        .withNode<GenericReconEigenChannelGadget>("coilcomp")
        .withNode<CmrCartesianKSpaceBinningCineGadget>("binning")
        .withNode<GenericReconPartialFourierHandlingPOCSGadget>("pf")
        .withNode<GenericReconKSpaceFilteringGadget>("kspace-filter")
        .withNode<GenericReconFieldOfViewAdjustmentGadget>("fov-adjust")
        .withNode<GenericReconImageArrayScalingGadget>("scale")
        .withNode<ImageArraySplitGadget>("split")
        .withNode<ComplexToFloatGadget>("complex-to-float")
        .withNode<FloatToFixedPointGadget>("convert")
        ;

static auto cmr_mapping_t1_sr = PipelineBuilder<Gadgetron::Core::MRContext>("cmr-mapping-t1-sr", "CMR 2DT T1 mapping SASHA")
        .withSource<MRSource>()
        .withSink<MRSink>()
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

static auto cmr_rtcine_lax_ai = PipelineBuilder<Gadgetron::Core::MRContext>("cmr-rtcine-lax-ai", "CMR real-time cine LAX AI")
        .withSource<MRSource>()
        .withSink<MRSink>()
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
        .withNode<CmrRealTimeLAXCineAIAnalysisGadget>("laxai")
        .withNode<ImageArraySplitGadget>("split")
        .withNode<ComplexToFloatGadget>("complex-to-float")
        .withNode<FloatToFixedPointGadget>("convert")
        ;

} // namespace pingvin