/** \file   GenericReconPartialFourierHandlingPOCSGadget.h
    \brief  This is the class gadget for both 2DT and 3DT cartesian partial fourier handling using the POCS.
    \author Hui Xue
*/

#pragma once

#include "GenericReconPartialFourierHandlingGadget.h"

namespace Gadgetron {

    class GenericReconPartialFourierHandlingPOCSGadget : public GenericReconPartialFourierHandlingGadget {
    public:
        typedef float real_value_type;
        typedef std::complex<real_value_type> ValueType;
        typedef ValueType T;

        typedef GenericReconPartialFourierHandlingGadget BaseClass;

        struct Parameters : public BaseClass::Parameters {
            Parameters(const std::string& prefix) : BaseClass::Parameters(prefix, "Partial Fourier Handling POCS")
            {
                register_parameter("partial-fourier-POCS-iters", &partial_fourier_POCS_iters,
                    "Number of iterations for POCS PF handling");
                register_parameter("partial-fourier-POCS-thres", &partial_fourier_POCS_thres,
                    "Threshold for POSC PF handling");
                register_parameter("partial-fourier-POCS-transitBand", &partial_fourier_POCS_transitBand,
                    "Transition band width for POCS PF handling");
                register_parameter("partial-fourier-POCS-transitBand-E2", &partial_fourier_POCS_transitBand_E2,
                    "Transition band width for POCS PF handling for E2 dimension");
            }

            size_t partial_fourier_POCS_iters = 6;
            double partial_fourier_POCS_thres = 0.01;
            size_t partial_fourier_POCS_transitBand = 24;
            size_t partial_fourier_POCS_transitBand_E2 = 16;
        };

        GenericReconPartialFourierHandlingPOCSGadget(const Core::MRContext& context, const Parameters& params)
            : BaseClass(context, params)
            , params_(params)
        {}

    protected:
        const Parameters params_;

        hoNDArray<std::complex<float>> perform_partial_fourier_handling(
            const hoNDArray<std::complex<float>>& kspace_buffer, size_t start_RO, size_t end_RO, size_t start_E1,
            size_t end_E1, size_t start_E2, size_t end_E2) const override;
    };
}
