/** \file   GenericReconCartesianNonLinearSpirit2DTGadget.h
    \brief  This is the class gadget for 2DT cartesian non-linear Spirit reconstruction, working on the ReconData.
            The redundant dimension is dimension N.

            Ref:
            Equation 3 and 4. [1] Hui Xue, Souheil Inati, Thomas Sangild Sorensen, Peter Kellman, Michael S. Hansen.
            Distributed MRI Reconstruction using Gadgetron based Cloud Computing. Magenetic Resonance in Medicine, 73(3):1015-25, 2015.

    \author Hui Xue
*/

#pragma once

#include "GenericReconCartesianSpiritGadget.h"

namespace Gadgetron {

    class GenericReconCartesianNonLinearSpirit2DTGadget : public GenericReconCartesianSpiritGadget
    {
    public:
        typedef GenericReconCartesianSpiritGadget BaseClass;
        typedef Gadgetron::GenericReconCartesianSpiritObj< std::complex<float> > ReconObjType;

        struct Parameters : public BaseClass::Parameters {
            Parameters(const std::string& prefix) : BaseClass::Parameters(prefix, "Cartesian Non-linear SPIRIT")
            {
                register_parameter("spirit-parallel-imaging-lamda", &spirit_parallel_imaging_lamda, "Spirit regularization strength for parallel imaging term");
                register_parameter("spirit-image-reg-lamda", &spirit_image_reg_lamda, "Spirit regularization strength for imaging term");
                register_parameter("spirit-data-fidelity-lamda", &spirit_data_fidelity_lamda, "Spirit regularization strength for data fidelity term");
                register_parameter("spirit-nl-iter-max", &spirit_nl_iter_max, "Spirit maximal number of iterations for nonlinear optimization");
                register_parameter("spirit-nl-iter-thres", &spirit_nl_iter_thres, "Spirit threshold to stop iteration for nonlinear optimization");
                register_parameter("spirit-reg-name", &spirit_reg_name, "Spirit image domain regularizer (db1, db2, db3, db4, db5)");
                register_parameter("spirit-reg-level", &spirit_reg_level, "Spirit image domain regularizer, number of transformation levels");
                register_parameter("spirit-reg-keep-approx-coeff", &spirit_reg_keep_approx_coeff, "Spirit whether to keep the approximation coefficients from being regularized");
                register_flag("spirit-reg-keep-redundant-dimension-coeff", &spirit_reg_keep_redundant_dimension_coeff, "Spirit whether to keep the boundary coefficients of N dimension from being regularized");
                register_flag("spirit-reg-proximity-across-cha", &spirit_reg_proximity_across_cha, "Spirit whether to perform proximity operation across channels");
                register_flag("spirit-reg-use-coil-sen-map", &spirit_reg_use_coil_sen_map, "Spirit whether to use coil map in the imaging term");
                register_flag("spirit-reg-estimate-noise-floor", &spirit_reg_estimate_noise_floor, "Spirit whether to estimate noise floor for the imaging term");
                register_parameter("spirit-reg-minimal-num-images-for-noise-floor", &spirit_reg_minimal_num_images_for_noise_floor, "Spirit minimal number of images for noise floor estimation");
                register_parameter("spirit-reg-RO-weighting-ratio", &spirit_reg_RO_weighting_ratio, "Spirit regularization weigthing ratio for RO");
                register_parameter("spirit-reg-E1-weighting-ratio", &spirit_reg_E1_weighting_ratio, "Spirit regularization weigthing ratio for E1");
                register_parameter("spirit-reg-N-weighting-ratio", &spirit_reg_N_weighting_ratio, "Spirit regularization weigthing ratio for N");
            }

            /// parameters for workflow
            // bool spirit_perform_linear = true; // "Spirit whether to perform linear reconstruction as the initialization"
            /// parameters for regularization
            double spirit_parallel_imaging_lamda = 1.0;
            double spirit_image_reg_lamda = 0;
            double spirit_data_fidelity_lamda = 1.0;
            /// parameters for non-linear iteration
            int spirit_nl_iter_max = 0;
            double spirit_nl_iter_thres = 0;
            /// parameters for image domain regularization, wavelet type regularizer is used here
            std::string spirit_reg_name = "db1";
            int spirit_reg_level = 1;
            bool spirit_reg_keep_approx_coeff = true;
            bool spirit_reg_keep_redundant_dimension_coeff = false;
            bool spirit_reg_proximity_across_cha = false;
            bool spirit_reg_use_coil_sen_map = false;
            bool spirit_reg_estimate_noise_floor = false;
            int spirit_reg_minimal_num_images_for_noise_floor = 16;
            /// W matrix of equation 3 and 4 in ref [1]
            double spirit_reg_RO_weighting_ratio = 1.0;
            double spirit_reg_E1_weighting_ratio = 1.0;
            double spirit_reg_N_weighting_ratio = 0;
        };

        GenericReconCartesianNonLinearSpirit2DTGadget(const Core::MrdContext& context, const Parameters& params);

    protected:
        Parameters params_;

        // --------------------------------------------------
        // variable for recon
        // --------------------------------------------------

        // --------------------------------------------------
        // recon step functions
        // --------------------------------------------------

        // unwrapping
        virtual void perform_unwrapping(mrd::ReconAssembly& recon_bit, ReconObjType& recon_obj, size_t encoding);

        // perform non-linear spirit unwrapping
        // kspace, kerIm, full_kspace: [RO E1 CHA N S SLC]
        void perform_nonlinear_spirit_unwrapping(hoNDArray< std::complex<float> >& kspace, hoNDArray< std::complex<float> >& kerIm, hoNDArray< std::complex<float> >& ref2DT, hoNDArray< std::complex<float> >& coilMap2DT, hoNDArray< std::complex<float> >& full_kspace, size_t e);
    };
}
