/** \file   GenericReconCartesianGrappaGadget.h
    \brief  This is the class gadget for both 2DT and 3DT cartesian grappa and grappaone reconstruction, working on the ReconData.
    \author Hui Xue
*/

#pragma once

#include "GenericReconGadget.h"

namespace Gadgetron {

    /// define the recon status
    template <typename T>
    class GenericReconCartesianGrappaObj
    {
    public:
        // ------------------------------------
        /// recon outputs
        // ------------------------------------
        /// reconstructed images, headers and meta attributes
        mrd::ImageArray recon_res_;

        /// gfactor, [RO E1 E2 uncombinedCHA+1 N S SLC]
        hoNDArray<typename realType<T>::Type> gfactor_;

        // ------------------------------------
        /// buffers used in the recon
        // ------------------------------------
        /// [RO E1 E2 srcCHA Nor1 Sor1 SLC]
        hoNDArray<T> ref_calib_;
        /// [RO E1 E2 dstCHA Nor1 Sor1 SLC]
        hoNDArray<T> ref_calib_dst_;

        /// reference data ready for coil map estimation
        /// [RO E1 E2 dstCHA Nor1 Sor1 SLC]
        hoNDArray<T> ref_coil_map_;

        /// for combined imgae channel
        /// convolution kernel, [RO E1 E2 srcCHA - uncombinedCHA dstCHA - uncombinedCHA Nor1 Sor1 SLC]
        hoNDArray<T> kernel_;
        /// image domain kernel, [RO E1 E2 srcCHA - uncombinedCHA dstCHA - uncombinedCHA Nor1 Sor1 SLC]
        hoNDArray<T> kernelIm_;
        /// image domain unmixing coefficients, [RO E1 E2 srcCHA - uncombinedCHA Nor1 Sor1 SLC]
        hoNDArray<T> unmixing_coeff_;

        /// coil sensitivity map, [RO E1 E2 dstCHA - uncombinedCHA Nor1 Sor1 SLC]
        hoNDArray<T> coil_map_;
    };
}

namespace Gadgetron {

    class GenericReconCartesianGrappaGadget : public GenericReconGadget
    {
    public:
        typedef GenericReconGadget BaseClass;
        typedef Gadgetron::GenericReconCartesianGrappaObj< std::complex<float> > ReconObjType;

        struct Parameters : BaseClass::Parameters {
            Parameters(const std::string& prefix) : Parameters(prefix, "Cartesian Grappa")
            {}

            Parameters(const std::string& prefix, const std::string& description) : BaseClass::Parameters(prefix, description)
            {
                register_flag("send-out-gfactor", &send_out_gfactor, "Whether to send out gfactor map");
                register_flag("send-out-snr-map", &send_out_snr_map, "Whether to send out SNR map");
                register_parameter("grappa-kSize-RO", &grappa_kSize_RO, "Grappa kernel size RO");
                register_parameter("grappa-kSize-E1", &grappa_kSize_E1, "Grappa kernel size E1");
                register_parameter("grappa-kSize-E2", &grappa_kSize_E2, "Grappa kernel size E2");
                register_parameter("grappa-reg-lamda", &grappa_reg_lamda, "Grappa regularization threshold");
                register_parameter("grappa-calib-over-determine-ratio", &grappa_calib_over_determine_ratio, "Grappa calibration overdermination ratio");
                register_parameter("downstream-coil-compression", &downstream_coil_compression, "Whether to perform downstream coil compression");
                register_parameter("downstream-coil-compression-thres", &downstream_coil_compression_thres, "Threadhold for downstream coil compression");
                register_parameter("downstream-coil-compression-num-modesKept", &downstream_coil_compression_num_modesKept, "Number of modes to keep for downstream coil compression");
            }
            bool send_out_gfactor = false;
            bool send_out_snr_map = false;
            int grappa_kSize_RO = 5;
            int grappa_kSize_E1 = 4;
            int grappa_kSize_E2 = 4;
            double grappa_reg_lamda = 0.0005;
            double grappa_calib_over_determine_ratio = 45;

            /// if downstream_coil_compression==true, down stream coil compression is used
            bool downstream_coil_compression = true;
            /// if downstream_coil_compression_num_modesKept > 0, this number of channels will be used as the dst channels
            double downstream_coil_compression_thres = 0.002;
            /// if downstream_coil_compression_num_modesKept==0 and downstream_coil_compression_thres>0, the number of dst channels will be determined  by this threshold
            size_t downstream_coil_compression_num_modesKept = 0;
        };

        GenericReconCartesianGrappaGadget(const Core::MRContext &context, const Parameters& params);
        ~GenericReconCartesianGrappaGadget() override;

    protected:
        const Parameters params_;

        // --------------------------------------------------
        // variable for recon
        // --------------------------------------------------
        // record the recon kernel, coil maps etc. for every encoding space
        std::vector< ReconObjType > recon_obj_;

        // --------------------------------------------------
        // gadget functions
        // --------------------------------------------------
        // default interface function
        virtual void process(Core::InputChannel< mrd::ReconData > &in, Core::OutputChannel &out) override;

        // --------------------------------------------------
        // recon step functions
        // --------------------------------------------------

        // if downstream coil compression is used, determine number of channels used and prepare the ref_calib_dst_
        virtual void prepare_down_stream_coil_compression_ref_data(const hoNDArray< std::complex<float> >& ref_src, hoNDArray< std::complex<float> >& ref_coil_map, hoNDArray< std::complex<float> >& ref_dst, size_t encoding);

        // calibration, if only one dst channel is prescribed, the GrappaOne is used
        virtual void perform_calib(mrd::ReconAssembly& recon_bit, ReconObjType& recon_obj, size_t encoding);

        // unwrapping or coil combination
        virtual void perform_unwrapping(mrd::ReconAssembly& recon_bit, ReconObjType& recon_obj, size_t encoding);

        // compute snr map
        virtual void compute_snr_map(ReconObjType& recon_obj, hoNDArray< std::complex<float> >& snr_map);

    };
}
