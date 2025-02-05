#pragma once

#include <map>
#include <memory>

#include "parallel/Merge.h"
#include "Channel.h"

namespace Gadgetron::Grappa {

    class Image {
    public:

        struct {
            uint32_t time_stamp;
            uint16_t slice;
            std::array<float, 3> position, read_dir, phase_dir, slice_dir, table_pos;
        } meta;

        hoNDArray<std::complex<float>> data;
    };

    class Weights {
    public:
        struct {
            uint16_t slice, n_combined_channels, n_uncombined_channels;
        } meta;

        hoNDArray<std::complex<float>> data;
    };

    class Unmixing : public Core::Parallel::MRMerge {
    public:
        struct Parameters : public Core::NodeParameters {
            using NodeParameters::NodeParameters;
            Parameters(const std::string& prefix) : NodeParameters(prefix, "Grappa Unmixing") {
                register_parameter("image-series", &image_series, "Image series number for output images");
                register_parameter("unmixing-scale", &unmixing_scale, "");
            }
            uint16_t image_series = 0;
            float unmixing_scale = 1.0;
        };

        Unmixing(const Core::MrdContext &context, const Parameters& params)
            : Core::Parallel::MRMerge(context, params)
            , header_(context.header)
            , image_dimensions(create_output_image_dimensions(context.header))
            , image_fov(create_output_image_fov(context.header)) {}

        void process(
                std::map<std::string, Core::GenericInputChannel> input,
                Core::OutputChannel output
        ) override;

    private:
        const Parameters parameters_;

        hoNDArray<std::complex<float>> unmix(const Image &image, const Weights &weights);

        std::vector<size_t> create_unmixed_image_dimensions(const Weights &weights);

        static std::vector<size_t> create_output_image_dimensions(const mrd::Header& header);
        static std::vector<float> create_output_image_fov(const mrd::Header& header);
        mrd::ImageHeader create_image_header(const Image &image, const Weights &weights);

        const mrd::Header header_;
        const std::vector<size_t> image_dimensions;
        const std::vector<float> image_fov;

        uint16_t image_index_counter = 0;
    };
}
