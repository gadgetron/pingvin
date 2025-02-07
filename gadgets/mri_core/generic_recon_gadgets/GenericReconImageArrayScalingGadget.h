/** \file   GenericReconGenericReconImageArrayScalingGadget.h
    \brief  This is the class gadget to scale the incoming mrd::ImageArray.

    \author Hui Xue
*/

#pragma once

#include "GenericReconBase.h"

#include "hoNDArray_utils.h"
#include "hoNDArray_elemwise.h"

namespace Gadgetron {

    class GenericReconImageArrayScalingGadget : public GenericReconImageArrayBase
    {
    public:
        typedef float real_value_type;
        typedef std::complex<real_value_type> ValueType;
        typedef ValueType T;

        typedef GenericReconImageArrayBase BaseClass;

        struct Parameters : public BaseClass::Parameters {
            Parameters(const std::string& prefix) : BaseClass::Parameters(prefix, "Image Array Scaling")
            {
                register_parameter("use-constant-scalingFactor", &use_constant_scalingFactor, "Whether to use constraint scaling; if not, the auto-scaling factor will be computed only ONCE");
                register_parameter("scalingFactor", &scalingFactor, "Default scaling ratio");
                register_parameter("min-intensity-value", &min_intensity_value, "Minimal intensity value for auto image scaling");
                register_parameter("max-intensity-value", &max_intensity_value, "Maximal intensity value for auto image scaling");
                register_parameter("auto-scaling-only-once", &auto_scaling_only_once, "Whether to compute auto-scaling factor only once; if false, an auto-scaling factor is computed for every incoming image array");

                register_parameter("use-dedicated-scalingFactor-meta-field", &use_dedicated_scalingFactor_meta_field, "If this meta field exists, scale the images with the dedicated scaling factor");
                register_parameter("scalingFactor-dedicated", &scalingFactor_dedicated, "Dedicated scaling ratio");
                register_parameter("scalingFactor-gfactor-map", &scalingFactor_gfactor_map, "Scaling ratio for gfactor map");
                register_parameter("scalingFactor-snr-map", &scalingFactor_snr_map, "Scaling ratio for snr map");
                register_parameter("scalingFactor-snr-std-map", &scalingFactor_snr_std_map, "Scaling ratio for snr standard deviation map");
            }

            bool use_constant_scalingFactor = true;
            float scalingFactor = 4.0;
            int min_intensity_value = 64;
            int max_intensity_value = 4095;
            bool auto_scaling_only_once = true;

            std::string use_dedicated_scalingFactor_meta_field = "Use_dedicated_scaling_factor";
            float scalingFactor_dedicated = 100.0;
            float scalingFactor_gfactor_map = 100.0;
            float scalingFactor_snr_map = 10.0;
            float scalingFactor_snr_std_map = 1000.0;
        };

        GenericReconImageArrayScalingGadget(const Core::MrdContext &context, const Parameters& params);

    protected:
        Parameters params_;

        // --------------------------------------------------
        // variables for protocol
        // --------------------------------------------------

        // --------------------------------------------------
        // variable for recon
        // --------------------------------------------------

        // scaling factor used for every encoding space
        std::vector<double> scaling_factor_;

        // --------------------------------------------------
        // functional functions
        // --------------------------------------------------

        // default interface function
        void process(Core::InputChannel< mrd::ImageArray >& in, Core::OutputChannel& out) override;

        // scale the recon images
        virtual int compute_and_apply_scaling_factor(mrd::ImageArray& res, size_t encoding);

    };
}
