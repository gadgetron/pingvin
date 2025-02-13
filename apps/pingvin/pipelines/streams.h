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
#include "gadgets/mri_core/AugmentImageMetadataGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconCartesianReferencePrepGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconEigenChannelGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconCartesianGrappaGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconPartialFourierHandlingFilterGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconKSpaceFilteringGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconFieldOfViewAdjustmentGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconImageArrayScalingGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconNoiseStdMapComputingGadget.h"

namespace pingvin {

static auto stream_cartesian_grappa_imagearray = PipelineBuilder<MrdContext>("stream-cartesian-grappa-imagearray", "Cartesian Grappa Recon to ImageArray")
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
        ;

static auto stream_cartesian_grappa = stream_cartesian_grappa_imagearray
        .duplicate("stream-cartesian-grappa", "Cartesian Grappa Recon to complex Image")
        .withNode<GenericReconImageArrayScalingGadget>("scale")
        .withNode<ImageArraySplitGadget>("split")
        .withNode<AugmentImageMetadataGadget>("augment-metadata")
        ;

static auto stream_image_array_scaling = PipelineBuilder<MrdContext>("stream-image-array-scaling", "Image Array Scaling")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<GenericReconImageArrayScalingGadget>("scale")
        ;

static auto stream_image_array_split = PipelineBuilder<MrdContext>("stream-image-array-split", "Image Array split")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<ImageArraySplitGadget>("split")
        ;

static auto stream_complex_to_float = PipelineBuilder<MrdContext>("stream-complex-to-float", "Complex to Float")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<ComplexToFloatGadget>("complex-to-float")
        ;

static auto stream_float_to_fixed_point = PipelineBuilder<MrdContext>("stream-float-to-fixed-point", "Float to Fixed-Point")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<FloatToFixedPointGadget>("convert")
        ;

} // namespace pingvin