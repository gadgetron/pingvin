#pragma once

#include <map>
#include <set>
#include <functional>

#include "mrd/types.h"

#include "Channel.h"

#include "hoNDArray.h"

namespace Gadgetron::Grappa {

    class AcquisitionBuffer {
    public:
        explicit AcquisitionBuffer(const mrd::Header& header);

        void add(const mrd::Acquisition& acquisition);

        template<class T>
        void add(const T &acquisitions) {
            for (const auto &acquisition : acquisitions) {
                add(acquisition);
            }
        }

        hoNDArray<std::complex<float>>
        take(size_t index);

        const hoNDArray<std::complex<float>> &
        view(size_t index) const;

        void clear(size_t index);

        bool is_sufficiently_sampled(size_t index) const;


        std::array<uint16_t,4> region_of_support(size_t index) const;

        void add_pre_update_callback(std::function<void(const mrd::Acquisition &)> fn);
        void add_post_update_callback(std::function<void(const mrd::Acquisition &)> fn);

    private:
        const mrd::Header header_;

        struct {
            std::set<uint32_t> expected_lines;
            size_t number_of_samples, number_of_lines;
            int line_offset = 0;
        } internals;

        struct buffer {
            hoNDArray<std::complex<float>> data;
            std::set<uint32_t> sampled_lines;
        };

        std::pair<uint32_t,uint32_t> fully_sampled_region(size_t slice) const;
        buffer create_buffer(const std::vector<size_t> &dimensions);

        std::map<size_t, buffer> buffers;

        const int minimum_calibration_region = 8;

        std::vector<std::function<void(const mrd::Acquisition &)>> pre_update_callbacks;
        std::vector<std::function<void(const mrd::Acquisition &)>> post_update_callbacks;
    };
}