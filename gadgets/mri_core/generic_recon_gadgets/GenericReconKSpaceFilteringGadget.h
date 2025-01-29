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

        GenericReconKSpaceFilteringGadget(const Core::Context &context, const Core::GadgetProperties &properties);

        /// ------------------------------------------------------------------------------------
        /// parameters to control the reconstruction
        /// ------------------------------------------------------------------------------------
        NODE_PROPERTY(skip_processing_meta_field, std::string, "If this meta field exists, pass the incoming image array to next gadget without processing", "Skip_processing_after_recon");

        /// ------------------------------------------------------------------------------------
        /// kspace filter parameters
        NODE_PROPERTY(filterRO, std::string, "Kspace filter for RO dimension (Gaussian, Hanning, TaperedHanning, None)", "Gaussian");

        NODE_PROPERTY(filterRO_sigma, double, "Filter sigma for gaussian for RO dimension", 1.0);
        NODE_PROPERTY(filterRO_width, double, "Filter width for tapered hanning for RO dimension", 0.15);

        // ------------------------------------------------------------------------------------

        NODE_PROPERTY(filterE1, std::string, "Kspace filter for E1 dimension (Gaussian, Hanning, TaperedHanning, None)", "Gaussian");

        NODE_PROPERTY(filterE1_sigma, double, "Filter sigma for gaussian for E1 dimension", 1.0);
        NODE_PROPERTY(filterE1_width, double, "Filter width for tapered hanning for E1 dimension", 0.15);

        // ------------------------------------------------------------------------------------

        NODE_PROPERTY(filterE2, std::string, "Kspace filter for E2 dimension (Gaussian, Hanning, TaperedHanning, None)", "Gaussian");

        NODE_PROPERTY(filterE2_sigma, double, "Filter sigma for gaussian for E2 dimension", 1.0);
        NODE_PROPERTY(filterE2_width, double, "Filter width for tapered hanning for E2 dimension", 0.15);

        // ------------------------------------------------------------------------------------

    protected:

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
