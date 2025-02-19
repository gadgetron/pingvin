/** \file   GenericReconGadget.h
    \brief  This serves an optional base class gadget for both 2DT and 3DT reconstruction, working on the ReconData.
            Some common functionalities are implemented here and can be reused in specific recon gadgets.
    \author Hui Xue
*/

#pragma once

#include "GenericReconBase.h"

#include "hoNDArray_utils.h"
#include "hoNDArray_elemwise.h"
#include "hoNDFFT.h"

#include "mri_core_coil_map_estimation.h"
#include "ImageArraySendMixin.h"

namespace Gadgetron {

    class GenericReconGadget : public ImageArraySendMixin<GenericReconGadget>, public GenericReconDataBase{
    public:
        typedef GenericReconDataBase BaseClass;

        struct Parameters : BaseClass::Parameters {
            Parameters(const std::string& prefix, const std::string& description)
                : BaseClass::Parameters(prefix, description)
            {
                register_parameter("image-series", &image_series, "Image series number");
                register_parameter("coil-map-algorithm", &coil_map_algorithm, "Method for coil map estimation (Inati, Inati_Iter)");
                register_parameter("coil-map-kernel-size-readout", &coil_map_kernel_size_readout, "Coil map estimation, kernel size along read out");
                register_parameter("coil-map-kernel-size-phase", &coil_map_kernel_size_phase, "Coil map estimation, kernel size along phase/slice encoding");
                register_parameter("coil-map-num-iter", &coil_map_num_iter, "Coil map estimation, number of iterations");
                register_parameter("coil-map-thres-iter", &coil_map_thres_iter, "Coil map estimation, threshold to stop iteration");
            }
            int image_series = 0;
            std::string coil_map_algorithm = "Inati";
            size_t coil_map_kernel_size_readout = 7;
            size_t coil_map_kernel_size_phase = 5;
            size_t coil_map_num_iter = 10;
            double coil_map_thres_iter = 1e-4;
        };

        GenericReconGadget(const Core::MRContext& context, const Parameters& params);

    protected:
        const Parameters params_;

        void send_out_image_array(mrd::ImageArray& res, size_t encoding, int series_num, const std::string& data_role, Core::OutputChannel& out);
        // --------------------------------------------------
        // variables for protocol
        // --------------------------------------------------

        // for every encoding space

        // acceleration factor for E1 and E2
        std::vector<double> acceFactorE1_;
        std::vector<double> acceFactorE2_;

        // calibration mode
        std::vector<mrd::CalibrationMode> calib_mode_;

        // --------------------------------------------------
        // variable for recon
        // --------------------------------------------------

        /// buffers used during recon
        hoNDArray< std::complex<float> > complex_im_recon_buf_;
        hoNDArray< std::complex<float> > data_recon_buf_;

        // filter used for ref coil map
        hoNDArray< std::complex<float> > filter_RO_ref_coi_map_;
        hoNDArray< std::complex<float> > filter_E1_ref_coi_map_;
        hoNDArray< std::complex<float> > filter_E2_ref_coi_map_;

        // kspace line offset in case of partial fourier
        std::vector<int> space_matrix_offset_E1_;
        std::vector<int> space_matrix_offset_E2_;

        // --------------------------------------------------
        // gadget functions
        // --------------------------------------------------
        // default interface function
        virtual void process(Core::InputChannel< mrd::ReconData >& in, Core::OutputChannel& out) override;

        // --------------------------------------------------
        // recon step functions
        // --------------------------------------------------

        // make the ref data for coil map estimation
        virtual void make_ref_coil_map(mrd::ReconBuffer& ref_, std::vector<size_t> recon_dims, hoNDArray< std::complex<float> >& ref_calib, hoNDArray< std::complex<float> >& ref_coil_map, size_t encoding);

        // estimate coil map
        virtual void perform_coil_map_estimation(const hoNDArray< std::complex<float> >& ref_coil_map, hoNDArray< std::complex<float> >& coil_map, size_t encoding);

        // compute image header
        virtual void compute_image_header(mrd::ReconAssembly& recon_bit, mrd::ImageArray& res, size_t encoding);

        // compute snr scaling factor from effective acceleration rate and sampling region
        void compute_snr_scaling_factor(mrd::ReconAssembly& recon_bit, float& effective_acce_factor, float& snr_scaling_ratio);

        // --------------------------------------------------
        // recon record functions
        // --------------------------------------------------

        // scan info
        float system_field_strength_T_;

        // protocol name
        std::string protocol_name_;

        // device name
        std::string device_;
        // patient ID
        std::string patient_;
        // study ID
        std::string study_;
        // measurement ID
        std::string measurement_;
        // patient position enum
        mrd::PatientPosition patient_position_;
        // acquired measurement ID
        std::string measurement_id_;
        // vendor name
        std::string vendor_;
    };
}
