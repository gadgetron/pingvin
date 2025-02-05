#pragma once

#include "pipeline.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"
#include "gadgets/mri_core/AcquisitionAccumulateTriggerGadget.h"
#include "gadgets/mri_core/BucketToBufferGadget.h"
#include "gadgets/mri_core/ExtractGadget.h"
#include "gadgets/mri_core/FFTGadget.h"
#include "gadgets/mri_core/CombineGadget.h"
#include "gadgets/mri_core/AutoScaleGadget.h"
#include "gadgets/mri_core/FloatToFixedPointGadget.h"

#include "gadgets/epi/EPIReconXGadget.h"
#include "gadgets/epi/EPICorrGadget.h"
#include "gadgets/epi/FFTXGadget.h"

namespace pingvin {

  using namespace Gadgetron;

  static auto epi_2d = PipelineBuilder<MrdContext>("epi", "Basic EPI Reconstruction")
                           .withSource<MrdSource>()
                           .withSink<MrdSink>()
                           .withNode<NoiseAdjustGadget>("noise-adjust")
                           .withNode<EPIReconXGadget>("reconx")
                           .withNode<EPICorrGadget>("epicorr")
                           .withNode<FFTXGadget>("fftx")
                           .withNode<AcquisitionAccumulateTriggerGadget>("acctrig")
                           .withNode<BucketToBufferGadget>("buffer")
                           .withNode<FFTGadget>("fft")
                           .withNode<CombineGadget>("combine")
                           .withNode<ExtractGadget>("extract")
                           .withNode<AutoScaleGadget>("autoscale")
                           .withNode<FloatToFixedPointGadget>("convert")
                           ;

} // namespace pingvin