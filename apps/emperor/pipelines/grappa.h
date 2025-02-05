#pragma once

#include "pipeline.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"
#include "gadgets/mri_core/PCACoilGadget.h"
#include "gadgets/mri_core/CoilReductionGadget.h"
#include "gadgets/mri_core/AsymmetricEchoAdjustROGadget.h"
#include "gadgets/mri_core/RemoveROOversamplingGadget.h"
#include "gadgets/grappa/SliceAccumulator.h"
#include "gadgets/grappa/AcquisitionFanout.h"
#include "gadgets/grappa/ImageAccumulator.h"
#include "gadgets/grappa/WeightsCalculator.h"
#include "gadgets/grappa/Unmixing.h"
#include "gadgets/mri_core/ExtractGadget.h"
#include "gadgets/mri_core/AutoScaleGadget.h"
#include "gadgets/mri_core/FloatToFixedPointGadget.h"

namespace pingvin {

using namespace Gadgetron;

static auto grappa = PipelineBuilder<MrdContext>("grappa", "Basic GRAPPA Reconstruction")
                        .withSource<MrdSource>()
                        .withSink<MrdSink>()
                        .withNode<NoiseAdjustGadget>("noise-adjust")
                        .withNode<PCACoilGadget>("pca")
                        .withNode<CoilReductionGadget>("coil-reduction")
                        .withNode<AsymmetricEchoAdjustROGadget>("echo-adjust")
                        .withNode<RemoveROOversamplingGadget>("remove-oversampling")
                        .withNode<Grappa::SliceAccumulator>("slice-acc")

                        // Parallel
                        //   Branch: AcquisitionFanout
                        //   Stream
                        //     ImageAccumulator
                        //   Stream
                        //     cpuWeightsCalculator
                        //   Merge: Unmixing
                        .withBranch<Grappa::AcquisitionFanout>("acq-fanout")
                            .withStream("images")
                                .withNode<Grappa::ImageAccumulator>("image-acc")
                            .withStream("weights")
                                .withNode<Grappa::cpuWeightsCalculator>("weights")
                            .withMerge<Grappa::Unmixing>("unmixing")

                        .withNode<ExtractGadget>("extract")
                        .withNode<AutoScaleGadget>("scale")
                        .withNode<FloatToFixedPointGadget>("convert");

} // namespace pingvin