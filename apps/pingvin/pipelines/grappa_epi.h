#pragma once

#include "pipeline.h"

#include "gadgets/epi/EPIReconXGadget.h"
#include "gadgets/epi/EPICorrGadget.h"
#include "gadgets/epi/FFTXGadget.h"
#include "gadgets/epi/OneEncodingGadget.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"
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

namespace pingvin {

  using namespace Gadgetron;

  static auto grappa_epi = PipelineBuilder<MrdContext>("grappa-epi", "Basic EPI Reconstruction")
                           .withSource<MrdSource>()
                           .withSink<MrdSink>()
                           .withNode<NoiseAdjustGadget>("noise")
                           .withNode<EPIReconXGadget>("reconx")
                           .withNode<EPICorrGadget>("epicorr")
                           .withNode<FFTXGadget>("fftx")
                           .withNode<OneEncodingGadget>("encoding")
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

} // namespace pingvin