#pragma once

#include "MRNode.h"
#include "hoNDArray.h"

#include <complex>
#include <map>

namespace Gadgetron {

    class AcquisitionAccumulateTriggerGadget
        : public Core::MRChannelGadget<std::variant<mrd::Acquisition, mrd::WaveformUint32>> {
    public:
        enum class TriggerDimension {
            kspace_encode_step_1,
            kspace_encode_step_2,
            average,
            slice,
            contrast,
            phase,
            repetition,
            set,
            segment,
            user_0,
            user_1,
            user_2,
            user_3,
            user_4,
            user_5,
            user_6,
            user_7,
            n_acquisitions,
            none
        };

        struct Parameters : public Core::NodeParameters {
            using NodeParameters::NodeParameters;
            Parameters(const std::string& prefix) : NodeParameters(prefix, "Acquisition Accumulate Trigger Options") {
                register_parameter("trigger-dimension", &trigger_dimension, "Dimension to trigger on");
                register_parameter("sorting-dimension", &sorting_dimension, "Dimension to sort on");
                register_parameter("n-acquisitions-before-trigger", &n_acquisitions_before_trigger,
                                   "Number of acquisition before first trigger");
                register_parameter("n-acquisitions-before-ongoing-trigger", &n_acquisitions_before_ongoing_trigger,
                                   "Number of acquisition before ongoing triggers");
            }
            TriggerDimension trigger_dimension = TriggerDimension::none;
            TriggerDimension sorting_dimension = TriggerDimension::none;
            unsigned long n_acquisitions_before_trigger = 40;
            unsigned long n_acquisitions_before_ongoing_trigger = 40;
        };

        AcquisitionAccumulateTriggerGadget(const Core::MRContext& context, const Parameters& params)
            : Core::MRChannelGadget<std::variant<mrd::Acquisition, mrd::WaveformUint32>>(context, params)
            , parameters_(params) {}

        void process(Core::InputChannel<std::variant<mrd::Acquisition, mrd::WaveformUint32>>& in,
            Core::OutputChannel& out) override;

        const Parameters parameters_;
        size_t trigger_events = 0;
    private:
        void send_data(Core::OutputChannel& out, std::map<unsigned int, mrd::AcquisitionBucket>& buckets,
                       std::vector<mrd::WaveformUint32>& waveforms);
    };

    std::ostream& operator<<(std::ostream& out, const AcquisitionAccumulateTriggerGadget::TriggerDimension& param);
    void validate(boost::any& v, const std::vector<std::string>& values, AcquisitionAccumulateTriggerGadget::TriggerDimension*, int);

    void from_string(const std::string& str, AcquisitionAccumulateTriggerGadget::TriggerDimension& val);

}
