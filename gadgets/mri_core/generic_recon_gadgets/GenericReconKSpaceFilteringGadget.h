/** \file   GenericReconKSpaceFilteringGadget.h
    \brief  This is the class gadget for both 2DT and 3DT cartesian kspace filtering, working on the ImageArray.

            This class is a part of general cartesian recon chain.

            The sampled kspace region is found from image meta in fields:

            sampling_limits_RO
            sampling_limits_E1
            sampling_limits_E2

\author Hui Xue
*/

#pragma once

#include "GenericReconBase.h"

namespace Gadgetron {

    class GenericReconKSpaceFilteringGadget : public GenericReconImageArrayBase
    {
    public:
        typedef GenericReconImageArrayBase BaseClass;

        struct Parameters : public BaseClass::Parameters {
            Parameters(const std::string& prefix) : BaseClass::Parameters(prefix, "KSpace Filtering")
            {
                register_parameter("skip-processing-meta-field", &skip_processing_meta_field, "If this meta field exists, pass the incoming image array to next gadget without processing");
                register_parameter("filterRO", &filterRO, "Kspace filter for RO dimension (Gaussian, Hanning, TaperedHanning, None)");
                register_parameter("filterRO-sigma", &filterRO_sigma, "Filter sigma for gaussian for RO dimension");
                register_parameter("filterRO-width", &filterRO_width, "Filter width for tapered hanning for RO dimension");
                register_parameter("filterE1", &filterE1, "Kspace filter for E1 dimension (Gaussian, Hanning, TaperedHanning, None)");
                register_parameter("filterE1-sigma", &filterE1_sigma, "Filter sigma for gaussian for E1 dimension");
                register_parameter("filterE1-width", &filterE1_width, "Filter width for tapered hanning for E1 dimension");
                register_parameter("filterE2", &filterE2, "Kspace filter for E2 dimension (Gaussian, Hanning, TaperedHanning, None)");
                register_parameter("filterE2-sigma", &filterE2_sigma, "Filter sigma for gaussian for E2 dimension");
                register_parameter("filterE2-width", &filterE2_width, "Filter width for tapered hanning for E2 dimension");
            }

            std::string skip_processing_meta_field = "Skip_processing_after_recon";

            std::string filterRO = "Gaussian";
            double filterRO_sigma = 1.0;
            double filterRO_width = 0.15;

            std::string filterE1 = "Gaussian";
            double filterE1_sigma = 1.0;
            double filterE1_width = 0.15;

            std::string filterE2 = "Gaussian";
            double filterE2_sigma = 1.0;
            double filterE2_width = 0.15;
        };

        GenericReconKSpaceFilteringGadget(const Core::MrdContext &context, const Parameters& params);

    protected:
        const Parameters params_;

        // --------------------------------------------------
        // variables for protocol
        // --------------------------------------------------

        // encoding FOV and recon FOV
        std::vector< std::vector<float> > encoding_FOV_;
        std::vector< std::vector<float> > recon_FOV_;

        // --------------------------------------------------
        // variable for recon
        // --------------------------------------------------

        // kspace filter for every encoding space
        std::vector< hoNDArray< std::complex<float> > > filter_RO_;
        std::vector< hoNDArray< std::complex<float> > > filter_E1_;
        std::vector< hoNDArray< std::complex<float> > > filter_E2_;

        // kspace buffer
        hoNDArray< std::complex<float> > kspace_buf_;

        // results of filtering
        hoNDArray< std::complex<float> > filter_res_;

        // --------------------------------------------------
        // functional functions
        // --------------------------------------------------

        // default interface function
        virtual void process(Core::InputChannel< mrd::ImageArray >& in, Core::OutputChannel& out) override;


        // find kspace sampled range
        void find_kspace_sampled_range(size_t min, size_t max, size_t len, size_t& r);
    };
}
