#pragma once

#include "Pipeline.h"

#include "MRSource.h"
#include "MRSink.h"

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

namespace Pingvin {

using namespace Gadgetron;

static auto grappa = PipelineBuilder<Gadgetron::Core::MRContext>("grappa", "Basic GRAPPA Reconstruction")
                        .withSource<MRSource>()
                        .withSink<MRSink>()
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
                        ;

static auto grappa_cpu = PipelineBuilder<Gadgetron::Core::MRContext>("grappa-cpu", "Basic GRAPPA Reconstruction")
                        .withSource<MRSource>()
                        .withSink<MRSink>()
                        .withNode<NoiseAdjustGadget>("noise-adjust")
                        .withNode<PCACoilGadget>("pca")
                        .withNode<CoilReductionGadget>("coil-reduction")
                        .withNode<AsymmetricEchoAdjustROGadget>("echo-adjust")
                        .withNode<RemoveROOversamplingGadget>("remove-oversampling")
                        .withNode<Grappa::SliceAccumulator>("slice-acc")

                        .withBranch<Grappa::AcquisitionFanout>("acq-fanout")
                            .withStream("images")
                                .withNode<Grappa::ImageAccumulator>("image-acc")
                            .withStream("weights")
                                .withNode<Grappa::gpuWeightsCalculator>("weights")
                            .withMerge<Grappa::Unmixing>("unmixing")

                        .withNode<ExtractGadget>("extract")
                        ;

} // namespace pingvin