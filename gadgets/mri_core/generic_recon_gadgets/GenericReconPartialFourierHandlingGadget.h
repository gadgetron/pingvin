/** \file   GenericReconPartialFourierHandlingGadget.h
    \brief  This is the class gadget for both 2DT and 3DT cartesian partial fourier handling, working on the ImageArray.

            This class is a part of general cartesian recon chain.

            The sampled kspace region is found from image meta in fields:

            sampling_limits_RO
            sampling_limits_E1
            sampling_limits_E2

    \author Hui Xue
*/

#pragma once


#include "hoNDArray_utils.h"
#include "hoNDArray_elemwise.h"
#include "hoNDFFT.h"

#include "mri_core_partial_fourier.h"
#include "MRPureNode.h"

namespace Gadgetron {

class GenericReconPartialFourierHandlingGadget : public Core::MRPureGadget<mrd::ImageArray, mrd::ImageArray>
    {
    public:
        typedef float real_value_type;
        typedef std::complex<real_value_type> ValueType;
        typedef ValueType T;
        using BaseClass = Core::MRPureGadget<mrd::ImageArray,mrd::ImageArray>;

        struct Parameters : public Core::NodeParameters {
            Parameters(const std::string& prefix, const std::string& description) : NodeParameters(prefix, description) {
                register_parameter("verbose", &verbose, "Verbose");
                register_parameter("perform-timing", &perform_timing, "Perform timing");
                register_parameter("skip-processing-meta-field", &skip_processing_meta_field, "If this meta field exists, pass the incoming image array to next gadget without processing");
            }

            bool verbose = false;
            bool perform_timing = false;
            std::string skip_processing_meta_field = "Skip_processing_after_recon";
        };

        GenericReconPartialFourierHandlingGadget(const Core::MRContext& context, const Parameters& params);

        virtual ~GenericReconPartialFourierHandlingGadget() = default;

        mrd::ImageArray process_function(mrd::ImageArray array) const override;

    protected:
        const Parameters params_;

        size_t num_encoding_spaces;
        // --------------------------------------------------
        // variables for protocol
        // --------------------------------------------------

        // acceleration factor for E1 and E2
        std::vector<double> acceFactorE1_;
        std::vector<double> acceFactorE2_;


        // --------------------------------------------------
        // implementation functions
        // --------------------------------------------------
        virtual hoNDArray <std::complex<float>> perform_partial_fourier_handling(const hoNDArray <std::complex<float>> &kspace_buffer, size_t start_RO, size_t end_RO,
                                         size_t start_E1, size_t end_E1, size_t start_E2, size_t end_E2) const  = 0;

    };
}
