#include "WeightsCalculator.h"

#include <set>

#include "common/AcquisitionBuffer.h"
#include "common/grappa_common.h"

#ifdef USE_CUDA
#include "gpu/WeightsCore.h"
#endif

#include "cpu/WeightsCore.h"

#include "SliceAccumulator.h"
#include "Unmixing.h"


namespace {
    using namespace Gadgetron;
    namespace Grappa = Gadgetron::Grappa;

    // A similar function should be available in the std library at some point.
    template <class T, std::size_t N>
    std::array<std::remove_cv_t<T>, N> to_array(T (&a)[N])
    {
        std::array<std::remove_cv_t<T>, N> array{};
        std::copy(std::begin(a), std::end(a), array.begin());
        return array;
    }

    std::vector<Grappa::Slice> take_available_slices(Core::InputChannel<Grappa::Slice> &input) {

        std::vector<Grappa::Slice> slices{};

        slices.emplace_back(input.pop());

        while(auto opt_slice = input.try_pop()) {
            slices.emplace_back(std::move(*opt_slice));
        }

        GDEBUG_STREAM("Read " << slices.size() << " available slice(s).");

        return std::move(slices);
    }



    class AccelerationMonitor {
    public:
        AccelerationMonitor(size_t max_slices) {
            previous_line = std::vector<std::optional<size_t>>(max_slices, std::nullopt);
            acceleration = std::vector<std::optional<size_t>>(max_slices, std::nullopt);
        }

        void operator()(const Grappa::AnnotatedAcquisition &acquisition) {

            if(previous_line[slice_of(acquisition)]) {
                if (line_of(acquisition) < previous_line[slice_of(acquisition)].value()) {
                    acceleration[slice_of(acquisition)] = std::nullopt;
                }
                else {
                    acceleration[slice_of(acquisition)] = line_of(acquisition) - previous_line[slice_of(acquisition)].value();
                }
            }
            previous_line[slice_of(acquisition)] = line_of(acquisition);
        }

        size_t acceleration_factor(size_t slice) const {
            return acceleration[slice].value();
        }

        void clear(size_t slice) {
            previous_line[slice] = acceleration[slice] = std::nullopt;
        }

    private:
        std::vector<std::optional<size_t>> previous_line;
        std::vector<std::optional<size_t>> acceleration;
    };

    class DirectionMonitor {
    public:
        explicit DirectionMonitor(Grappa::AcquisitionBuffer &buffer,  AccelerationMonitor &acceleration, size_t max_slices)
        : buffer(buffer),  acceleration(acceleration), orientations(max_slices) {

        }

        void operator()(const Grappa::AnnotatedAcquisition &acquisition) {

            auto header = std::get<mrd::Acquisition>(acquisition).head;

            auto& [position, read_dir, slice_dir,phase_dir] = orientations.at(slice_of(acquisition));

            std::array<float, 3> header_position;
            std::copy(std::begin(header.position), std::end(header.position), std::begin(header_position));
            std::array<float, 3> header_read_dir;
            std::copy(std::begin(header.read_dir), std::end(header.read_dir), std::begin(header_read_dir));
            std::array<float, 3> header_phase_dir;
            std::copy(std::begin(header.phase_dir), std::end(header.phase_dir), std::begin(header_phase_dir));
            std::array<float, 3> header_slice_dir;
            std::copy(std::begin(header.slice_dir), std::end(header.slice_dir), std::begin(header_slice_dir));

            if (position == header_position &&
                read_dir == header_read_dir &&
                phase_dir == header_phase_dir &&
                slice_dir == header_slice_dir) {
                return;
            }

            position = header_position;
            read_dir = header_read_dir;
            phase_dir = header_phase_dir;
            slice_dir = header_slice_dir;

            clear(slice_of(acquisition));
        }

        void clear(size_t slice) {
            buffer.clear(slice);
            acceleration.clear(slice);
        }


    private:
        Grappa::AcquisitionBuffer &buffer;
        AccelerationMonitor &acceleration;
        struct SliceOrientation {
            std::array<float, 3> position = {0,0,0};
            std::array<float, 3> read_dir = {0,0,0};
            std::array<float, 3> slice_dir = {0,0,0};
            std::array<float, 3> phase_dir = {0,0,0};
        };

        std::vector<SliceOrientation> orientations;
    };
}

namespace Gadgetron::Grappa {

    template<class WeightsCore>
    Grappa::Weights create_weights(
            uint16_t index,
            const AcquisitionBuffer &buffer,
            uint16_t n_combined_channels,
            uint16_t n_uncombined_channels,
            const AccelerationMonitor &acceleration_monitor,
            WeightsCore &core
    ) {
        return Grappa::Weights{
                {
                        index,
                        n_combined_channels,
                        n_uncombined_channels
                },
                core.calculate_weights(
                        buffer.view(index),
                        buffer.region_of_support(index),
                        acceleration_monitor.acceleration_factor(index),
                        n_combined_channels,
                        n_uncombined_channels
                )
        };
    }

    template<class WeightsCore>
    WeightsCalculator<WeightsCore>::WeightsCalculator(
            const Core::Context &context,
            const std::unordered_map<std::string, std::string> &props
    ) : ChannelGadget<Grappa::Slice>(context,props), context(context) {}

    template<class WeightsCore>
    void WeightsCalculator<WeightsCore>::process(Core::InputChannel<Grappa::Slice> &in, Core::OutputChannel &out) {

        std::set<uint16_t> updated_slices{};
        uint16_t n_combined_channels = 0, n_uncombined_channels = 0;

        const auto slice_limits = context.header.encoding[0].encoding_limits.slice;
        const size_t max_slices = slice_limits ? context.header.encoding[0].encoding_limits.slice->maximum+1 : 1;

        AcquisitionBuffer buffer{context};
        AccelerationMonitor acceleration_monitor{max_slices};

        buffer.add_pre_update_callback(DirectionMonitor{buffer, acceleration_monitor,max_slices});
        buffer.add_post_update_callback([&](auto &acq) { updated_slices.insert(slice_of(acq)); });
        buffer.add_post_update_callback([&](auto &acq) { acceleration_monitor(acq); });
        buffer.add_post_update_callback([&](auto &acq) {
            n_combined_channels = combined_channels(acq);
            n_uncombined_channels = uncombined_channels(acq);
        });

        WeightsCore core{
                {coil_map_estimation_ks, coil_map_estimation_power},
                {block_size_samples, block_size_lines, convolution_kernel_threshold}
        };

        while (true) {
            auto slices = take_available_slices(in);
            buffer.add(slices);

            for (auto index : updated_slices) {

                if (!buffer.is_sufficiently_sampled(index)) continue;
                out.push(create_weights(
                        index,
                        buffer,
                        n_combined_channels,
                        n_uncombined_channels,
                        acceleration_monitor,
                        core
                ));
            }
            updated_slices.clear();
        }
    }

    using cpuWeightsCalculator = WeightsCalculator<CPU::WeightsCore>;
    GADGETRON_GADGET_EXPORT(cpuWeightsCalculator);

#ifdef USE_CUDA
    using gpuWeightsCalculator = WeightsCalculator<GPU::WeightsCore>;
    GADGETRON_GADGET_EXPORT(gpuWeightsCalculator);
#endif
}