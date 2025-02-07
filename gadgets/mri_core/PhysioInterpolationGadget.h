#pragma once

#include "Node.h"
#include "hoNDArray.h"

#include <complex>

namespace Gadgetron{

enum class PhysioInterpolationMode {
    separate,
    complete
};

enum class PhysioInterpolationMethod {
    Spline,
    BSpline
};


inline void from_string(const std::string& str, PhysioInterpolationMode& mode ){
    if (str == "separate" || str == "0" ) mode = PhysioInterpolationMode::separate;
    else if (str == "complete" || str == "1") mode = PhysioInterpolationMode::complete;
    else throw std::invalid_argument(str + " is not a valid PhysioInterpolationMode");
}

inline void from_string(const std::string& str, PhysioInterpolationMethod& method ){
    if (str == "Spline") method = PhysioInterpolationMethod::Spline;
    else if (str == "BSpline") method = PhysioInterpolationMethod::BSpline;
    else throw std::invalid_argument(str + " is not a valid input for PhysioInterpolationMethod");
}

std::ostream& operator<<(std::ostream& out, const PhysioInterpolationMode& mode) {
    switch (mode) {
        case PhysioInterpolationMode::separate: out << "separate"; break;
        case PhysioInterpolationMode::complete: out << "complete"; break;
    }
    return out;
}

void validate(boost::any& v, const std::vector<std::string>& values, PhysioInterpolationMode*, int) {
    // Make sure no previous assignment to 'a' was made.
    po::validators::check_first_occurrence(v);
    // Extract the first string from 'values'. If there is more than
    // one string, it's an error, and exception will be thrown.
    const std::string& s = po::validators::get_single_string(values);

    PhysioInterpolationMode mode;
    try {
        Gadgetron::from_string(s, mode);
    } catch (std::exception& e) {
        throw po::validation_error(po::validation_error::invalid_option_value);
    }
    v = boost::any(mode);
}

std::ostream& operator<<(std::ostream& out, const PhysioInterpolationMethod& method) {
    switch (method) {
        case PhysioInterpolationMethod::Spline: out << "Spline"; break;
        case PhysioInterpolationMethod::BSpline: out << "BSpline"; break;
    }
    return out;
}

void validate(boost::any& v, const std::vector<std::string>& values, PhysioInterpolationMethod*, int) {
    // Make sure no previous assignment to 'a' was made.
    po::validators::check_first_occurrence(v);
    // Extract the first string from 'values'. If there is more than
    // one string, it's an error, and exception will be thrown.
    const std::string& s = po::validators::get_single_string(values);

    PhysioInterpolationMethod method;
    try {
        Gadgetron::from_string(s, method);
    } catch (std::exception& e) {
        throw po::validation_error(po::validation_error::invalid_option_value);
    }
    v = boost::any(method);
}


class PhysioInterpolationGadget : public Core::MRChannelGadget<mrd::Image<std::complex<float>>>
    {
    public:
        struct Parameters : public Core::NodeParameters {
            int physiology_time_index = 0;
            PhysioInterpolationMode mode = PhysioInterpolationMode::separate;
            unsigned short phases = 30;
            bool first_beat_on_trigger = false;
            PhysioInterpolationMethod interp_method = PhysioInterpolationMethod::Spline;
            double time_stamp_resolution = 2.5;

            Parameters(const std::string& prefix)
                : Core::NodeParameters(prefix, "Interpolates images based on physiological data")
            {
                register_parameter("physiology-time-index", &physiology_time_index, "Physiology time index");
                register_parameter("mode", &mode, "Mode, 0=separate series for each RR, 1=First complete RR only");
                register_parameter("phases", &phases, "Number of cardiac phases");
                register_parameter("first-beat-on-trigger", &first_beat_on_trigger, "Indicates that acquisition was started on trigger");
                register_parameter("interp-method", &interp_method, "Interpolation method");
                register_parameter("time-stamp-resolution", &time_stamp_resolution, "Time stamp resolution in ms");
            }
        };

        PhysioInterpolationGadget(const Core::MrdContext& context, const Parameters& params)
            : Core::MRChannelGadget<mrd::Image<std::complex<float>>>(context, params)
            , parameters_(params)
            {
                mrd::EncodingLimitsType e_limits = context.header.encoding[0].encoding_limits;
                slice_limit_ = e_limits.slice ? e_limits.slice->maximum + 1 : 1;
            }

        void process(Core::InputChannel<mrd::Image<std::complex<float>>>& in, Core::OutputChannel& out) override;

    protected:
        const Parameters parameters_;

        uint32_t slice_limit_;
    };

}