/**
    \brief  Autoscales real-type images based on a given max value and the 99th percentile of the data
    \test   Tested by: epi_2d.cfg and others
*/

#pragma once

#include "PureGadget.h"
#include "hoNDArray_math.h"
#include <algorithm>

namespace Gadgetron{
    class AutoScaleGadget : public Core::MRPureGadget<mrd::AnyImage, mrd::AnyImage> {
    public:
        struct Parameters : public Core::NodeParameters {
            float max_value = 2048;
            unsigned int histogram_bins = 100;

            Parameters(const std::string& prefix): Core::NodeParameters(prefix, "Scale images to a given percentile") {
                register_parameter("max_value", &max_value, "Maximum value (after scaling)");
                register_parameter("histogram_bins", &histogram_bins, "Number of Histogram Bins");
            }
        };

        AutoScaleGadget(const Core::MrdContext& context, const Parameters& params)
            : Core::MRPureGadget<mrd::AnyImage, mrd::AnyImage >(context, params)
            , parameters_(params)
        {}

        mrd::AnyImage process_function(mrd::AnyImage image) const override;

    protected:
        const Parameters parameters_;

        float current_scale_;
        std::vector<size_t> histogram_;
    };
}
