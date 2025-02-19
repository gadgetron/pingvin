/** \file   GenericReconCartesianSpiritGadget.h
    \brief  This is the class gadget for both 2DT and 3DT cartesian Spirit reconstruction, working on the ReconData.
    \author Hui Xue
*/

#pragma once

#include "GenericReconGadget.h"

namespace Gadgetron {

    /// define the recon status
    template <typename T>
    class GenericReconCartesianSpiritObj
    {
    public:

        GenericReconCartesianSpiritObj() {}
        virtual ~GenericReconCartesianSpiritObj() {}

        // ------------------------------------
        /// recon outputs
        // ------------------------------------
        /// reconstructed images, headers and meta attributes
        mrd::ImageArray recon_res_;

        /// full kspace reconstructed
        hoNDArray<T> full_kspace_;

        // ------------------------------------
        /// buffers used in the recon
        // ------------------------------------
        /// [RO E1 E2 dstCHA Nor1 Sor1 SLC]
        hoNDArray<T> ref_calib_;

        /// reference data ready for coil map estimation
        /// [RO E1 E2 dstCHA Nor1 Sor1 SLC]
        hoNDArray<T> ref_coil_map_;

        /// for combined imgae channel
        /// convolution kernel, [convRO convE1 convE2 dstCHA dstCHA Nor1 Sor1 SLC]
        hoNDArray<T> kernel_;
        /// image domain kernel for 2D, [RO E1 dstCHA dstCHA Nor1 Sor1 SLC]
        hoNDArray<T> kernelIm2D_;

        /// due to the iterative nature of SPIRIT method, the complete memory storage of 3D kernel is not feasible
        /// the RO decouplling is used for 3D spirit
        /// image domain kernel 3D, [convE1 convE2 dstCHA dstCHA RO Nor1 Sor1 SLC]
        hoNDArray<T> kernelIm3D_;

        /// coil sensitivity map, [RO E1 E2 dstCHA Nor1 Sor1 SLC]
        hoNDArray<T> coil_map_;

        /// an estimate of gfactor
        /// gfactor, [RO E1 E2 1 N S SLC]
        hoNDArray<typename realType<T>::Type> gfactor_;
    };
}

namespace Gadgetron {

    class GenericReconCartesianSpiritGadget : public GenericReconGadget
    {
    public:
        typedef GenericReconGadget BaseClass;
        typedef Gadgetron::GenericReconCartesianSpiritObj< std::complex<float> > ReconObjType;

        struct Parameters : public BaseClass::Parameters {
            Parameters(const std::string& prefix) : Parameters(prefix, "Cartesian SPIRIT")
            {}

            Parameters(const std::string& prefix, const std::string& description)
                : BaseClass::Parameters(prefix, description)
            {
                register_parameter("spirit-kSize-RO", &spirit_kSize_RO, "Spirit kernel size RO");
                register_parameter("spirit-kSize-E1", &spirit_kSize_E1, "Spirit kernel size E1");
                register_parameter("spirit-kSize-E2", &spirit_kSize_E2, "Spirit kernel size E2");
                register_parameter("spirit-reg-lamda", &spirit_reg_lamda, "Spirit regularization threshold");
                register_parameter("spirit-calib-over-determine-ratio", &spirit_calib_over_determine_ratio, "Spirit calibration overdermination ratio");
                register_parameter("spirit-iter-max", &spirit_iter_max, "Spirit maximal number of iterations");
                register_parameter("spirit-iter-thres", &spirit_iter_thres, "Spirit threshold to stop iteration");
                register_flag("spirit-print-iter", &spirit_print_iter, "Spirit print out iterations");
            }

            int spirit_kSize_RO = 7;
            int spirit_kSize_E1 = 7;
            int spirit_kSize_E2 = 5;
            double spirit_reg_lamda = 0;
            double spirit_calib_over_determine_ratio = 45;
            int spirit_iter_max = 0;
            double spirit_iter_thres = 0;
            bool spirit_print_iter = false;
        };

        GenericReconCartesianSpiritGadget(const Core::MRContext& context, const Parameters& params);

    protected:
        Parameters params_;

        // --------------------------------------------------
        // variable for recon
        // --------------------------------------------------
        // record the recon kernel, coil maps etc. for every encoding space
        std::vector< ReconObjType > recon_obj_;

        // --------------------------------------------------
        // gadget functions
        // --------------------------------------------------
        // default interface function
        void process(Core::InputChannel<mrd::ReconData>& in, Core::OutputChannel& out) override;

        // --------------------------------------------------
        // recon step functions
        // --------------------------------------------------

        // calibration, if only one dst channel is prescribed, the SpiritOne is used
        virtual void perform_calib(mrd::ReconAssembly& recon_bit, ReconObjType& recon_obj, size_t encoding);

        // unwrapping or coil combination
        virtual void perform_unwrapping(mrd::ReconAssembly& recon_bit, ReconObjType& recon_obj, size_t encoding);

        // perform spirit unwrapping
        // kspace, kerIm, full_kspace: [RO E1 CHA N S SLC]
        void perform_spirit_unwrapping(hoNDArray< std::complex<float> >& kspace, hoNDArray< std::complex<float> >& kerIm, hoNDArray< std::complex<float> >& full_kspace);

        // perform coil combination
        void perform_spirit_coil_combine(ReconObjType& recon_obj);
    };
}
