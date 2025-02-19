#pragma once

#include "Pipeline.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"
#include "gadgets/mri_core/AsymmetricEchoAdjustROGadget.h"
#include "gadgets/mri_core/RemoveROOversamplingGadget.h"
#include "gadgets/mri_core/AcquisitionAccumulateTriggerGadget.h"
#include "gadgets/mri_core/BucketToBufferGadget.h"
#include "gadgets/mri_core/ImageArraySplitGadget.h"
#include "gadgets/mri_core/PhysioInterpolationGadget.h"
#include "gadgets/mri_core/ComplexToFloatGadget.h"
#include "gadgets/mri_core/FloatToFixedPointGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconCartesianReferencePrepGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconEigenChannelGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconCartesianNonLinearSpirit2DTGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconPartialFourierHandlingPOCSGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconKSpaceFilteringGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconFieldOfViewAdjustmentGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconImageArrayScalingGadget.h"

namespace Pingvin {

static auto cartesian_spirit = PipelineBuilder<Gadgetron::Core::MrdContext>("cartesian-spirit", "Cartesian SPIRIT Recon")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<NoiseAdjustGadget>("noise")
        .withNode<AsymmetricEchoAdjustROGadget>("echo-adjust")
        .withNode<RemoveROOversamplingGadget>("ros")
        .withNode<AcquisitionAccumulateTriggerGadget>("acctrig")
        .withNode<BucketToBufferGadget>("buffer")
        .withNode<GenericReconCartesianReferencePrepGadget>("refprep")
        .withNode<GenericReconEigenChannelGadget>("coilcomp")
        .withNode<GenericReconCartesianSpiritGadget>("spirit")
        .withNode<GenericReconPartialFourierHandlingFilterGadget>("partial-fourier")
        .withNode<GenericReconKSpaceFilteringGadget>("kspace-filter")
        .withNode<GenericReconFieldOfViewAdjustmentGadget>("fov-adjust")
        .withNode<GenericReconImageArrayScalingGadget>("scale")
        .withNode<ImageArraySplitGadget>("split")
        .withNode<ComplexToFloatGadget>("complex-to-float")
        .withNode<FloatToFixedPointGadget>("convert")
        ;

static auto cartesian_spirit_nonlinear = PipelineBuilder<Gadgetron::Core::MrdContext>("cartesian-nonlinear-spirit", "Cartesian NonLinear Spirit RealTimeCine")
        .withSource<MrdSource>()
        .withSink<MrdSink>()
        .withNode<NoiseAdjustGadget>("noise")
        .withNode<AsymmetricEchoAdjustROGadget>("echo-adjust")
        .withNode<RemoveROOversamplingGadget>("ros")
        .withNode<AcquisitionAccumulateTriggerGadget>("acctrig")
        .withNode<BucketToBufferGadget>("buffer")
        .withNode<GenericReconCartesianReferencePrepGadget>("refprep")
        .withNode<GenericReconEigenChannelGadget>("coil-comp")
        .withNode<GenericReconCartesianNonLinearSpirit2DTGadget>("spirit")
        .withNode<GenericReconPartialFourierHandlingPOCSGadget>("partial-fourier")
        .withNode<GenericReconKSpaceFilteringGadget>("kspace-filter")
        .withNode<GenericReconFieldOfViewAdjustmentGadget>("fov-adjust")
        .withNode<GenericReconImageArrayScalingGadget>("scale")
        .withNode<ImageArraySplitGadget>("image-split")
        .withNode<PhysioInterpolationGadget>("physio-interp")
        .withNode<ComplexToFloatGadget>("complex-to-float")
        .withNode<FloatToFixedPointGadget>("convert")
        ;

} // namespace pingvin