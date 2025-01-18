#pragma once

#include "PureGadget.h"

namespace Gadgetron {
    using PercentileScaleImageTypes = std::variant<mrd::Image<float>, mrd::ImageArray>;
/***
 * This Gadget rescales magnitude images so that their 99% percentile becomes max_value
 */
    class ScaleGadget : public Core::MRPureGadget<PercentileScaleImageTypes, PercentileScaleImageTypes > {
    public:
        struct Parameters : public Core::NodeParameters {
            float max_value = 2048;
            float percentile = 98.5;

            Parameters(const std::string& prefix): Core::NodeParameters(prefix, "Scale images to a given percentile") {
                register_parameter("max_value", &max_value, "Percentile value (after scaling)");
                register_parameter("percentile", &percentile, "Percentile to use.");
            }
        };

        ScaleGadget(const Core::MrdContext& context, const Parameters& params)
            : Core::MRPureGadget<PercentileScaleImageTypes, PercentileScaleImageTypes >(context, params)
            , parameters_(params)
        {}

       PercentileScaleImageTypes process_function(PercentileScaleImageTypes args) const override;

    protected:
        const Parameters parameters_;
    };
}


