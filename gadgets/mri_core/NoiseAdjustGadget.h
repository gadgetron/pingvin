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
        using Core::ChannelGadget<mrd::Acquisition>::ChannelGadget;
        NoiseAdjustGadget() : ChannelGadget("noise_adjust") {}
        NoiseAdjustGadget(const Core::Context& context, const Core::GadgetProperties& props);

        void install_cli(po::options_description& options) override;

        void process(Core::InputChannel<mrd::Acquisition>& in, Core::OutputChannel& out) override;

        using NoiseHandler = std::variant<NoiseGatherer, LoadedNoise, Prewhitener, IgnoringNoise>;

    protected:
        NODE_PROPERTY(perform_noise_adjust, bool, "Whether to actually perform the noise adjust", true);
        NODE_PROPERTY(pass_nonconformant_data, bool, "Whether to pass data that does not conform", true);
        NODE_PROPERTY(noise_dwell_time_us_preset, float, "Preset dwell time for noise measurement", 0.0);
        NODE_PROPERTY(scale_only_channels_by_name, std::string, "List of named channels that should only be scaled", "");

        void initialize_(const Core::Context& context) override;

        // const float receiver_noise_bandwidth;
        float receiver_noise_bandwidth;

        // const std::string measurement_id;
        std::string measurement_id;
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

        // File names for file storage and retrival of noise covariance
        std::string noise_covariance_out = "";
        std::string noise_covariance_in = "";
    };
}

