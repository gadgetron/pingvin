#pragma once

#include "GadgetronTimer.h"
#include "Node.h"
#include "hoNDArray.h"

#include <complex>

namespace Gadgetron {

    struct NoiseGatherer {
        hoNDArray<std::complex<float>> tmp_covariance;
        size_t normalized_number_of_samples = 0;
        size_t total_number_of_samples = 0;
        float noise_dwell_time_us = 0;
    };

    struct Prewhitener {
        hoNDArray<std::complex<float>> prewhitening_matrix;
    };

    struct LoadedNoise {
        hoNDArray<std::complex<float>> covariance;
        float noise_dwell_time_us;
    };

    struct IgnoringNoise {};

    class NoiseAdjustGadget : public Core::ChannelGadget<mrd::Acquisition> {
    public:
        struct Parameters : public Core::NodeParameters {
            using NodeParameters::NodeParameters;
            // File names for file storage and retrival of noise covariance
            std::string noise_covariance_in = "";
            std::string noise_covariance_out = "";

            bool skip_noise_adjust = false;
            bool discard_nonconformant_data = false;
            float noise_dwell_time_us_preset = 0.0;
            std::string scale_only_channels_by_name = "";

            Parameters(const std::string& prefix) : NodeParameters(prefix, "Noise Adjustment Options") {
                register_parameter("covariance-input", &noise_covariance_in, "Input file containing noise covariance matrix");
                register_parameter("covariance-output", &noise_covariance_out, "Output file containing noise covariance matrix");
                register_flag("skip", &skip_noise_adjust, "Skip noise adjustment");
                register_flag("discard-nonconformant-data", &discard_nonconformant_data, "Discard data that does not conform");
                register_parameter("dwell-time-us-preset", &noise_dwell_time_us_preset, "Preset dwell time for noise measurement");
                register_parameter("scale-only-channels-by-name", &scale_only_channels_by_name, "List of named channels that should only be scaled");
            }
        };

        NoiseAdjustGadget(const Core::MrdContext& context, const Parameters& params);

        void process(Core::InputChannel<mrd::Acquisition>& in, Core::OutputChannel& out) override;

        using NoiseHandler = std::variant<NoiseGatherer, LoadedNoise, Prewhitener, IgnoringNoise>;

    protected:
        const Parameters parameters_;

        const float receiver_noise_bandwidth;

        const std::string measurement_id;
        std::vector<size_t> scale_only_channels;

        NoiseHandler noisehandler = IgnoringNoise{};

        template<class NOISEHANDLER>
        void add_noise(NOISEHANDLER& nh, const mrd::Acquisition&) const ;

        template<class NOISEHANDLER>
        NoiseHandler handle_acquisition(NOISEHANDLER nh, mrd::Acquisition&);

        std::optional<mrd::NoiseCovariance> load_noisedata() const;

        template<class NOISEHANDLER>
        void save_noisedata(NOISEHANDLER& nh);

        NoiseHandler load_or_gather() const;
    };
}

