#pragma once

#include "SliceAccumulator.h"

#include "MRNode.h"

#include "cpu/WeightsCore.h"
#ifdef USE_CUDA
#include "gpu/WeightsCore.h"
#endif

namespace Gadgetron::Grappa {

    template<class WeightsCore>
    class WeightsCalculator : public Core::MRChannelGadget<Slice> {
    public:
        struct Parameters : public Core::NodeParameters {
            uint16_t coil_map_estimation_ks = 5;
            uint16_t coil_map_estimation_power = 3;

            uint16_t block_size_lines = 4;
            uint16_t block_size_samples = 5;
            float convolution_kernel_threshold = 5e-4;

            Parameters(const std::string &prefix)
                    : Core::NodeParameters(prefix, "Grappa Weights Calculator") {
                register_parameter("coil-map-estimation-ks", &coil_map_estimation_ks, "Kernel size for coil map estimation");
                register_parameter("coil-map-estimation-power", &coil_map_estimation_power, "Power for coil map estimation");

                register_parameter("block-size-lines", &block_size_lines, "Block size used to estimate missing samples; number of lines");
                register_parameter("block-size-samples", &block_size_samples, "Block size used to estimate missing samples; number of samples");
                register_parameter("convolution-kernel-threshold", &convolution_kernel_threshold, "Grappa convolution kernel calibration Tikhonov threshold");
            }
        };

        WeightsCalculator(const Core::MRContext &context, const Parameters &params)
            : Core::MRChannelGadget<Slice>(context, params)
            , parameters_(params)
            , header_(context.header)
        {}

        void process(Core::InputChannel<Slice> &in, Core::OutputChannel &out) override;

    private:
        const Parameters parameters_;
        const mrd::Header header_;
    };

    using cpuWeightsCalculator = WeightsCalculator<CPU::WeightsCore>;

    #ifdef USE_CUDA
        using gpuWeightsCalculator = WeightsCalculator<GPU::WeightsCore>;
    #endif
}
