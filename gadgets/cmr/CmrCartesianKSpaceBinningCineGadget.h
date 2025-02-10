/** \file   CmrCartesianKSpaceBinningCineGadget.h
    \brief  This is the class gadget for 2DT cartesian KSpace binning reconstruction, working on the mrd::ReconData.
            The temporal dimension is dimension N and motion sharing dimension is S.

            Ref:
            [1] Hui Xue, Peter Kellman, Gina LaRocca, Andrew E Arai and Michael S Hansen.
                High spatial and temporal resolution retrospective cine cardiovascular magnetic resonance from shortened free breathing real-time acquisitions.
                Journal of Cardiovascular Magnetic Resonance 2013; 15:102.

            [2] Peter Kellman, Christophe Chefd’hotel, Christine H. Lorenz, Christine Mancini, Andrew E. Arai, Elliot R. McVeigh.
                High spatial and temporal resolution cardiac cine MRI from retrospective reconstruction of data acquired in real time using motion correction and resorting.
                Magn Reson Med. 2009; 62:1557–64.

            [3] Michael S. Hansen, Thomas S. Sørensen, Andrew E. Arai, and Peter Kellman.
                Retrospective reconstruction of high temporal resolution cine images from real-time MRI using iterative motion correction.
                Magn Reson Med. 2012; 68:741–50.

    \author Hui Xue
*/

#pragma once

#include "generic_recon_gadgets/GenericReconGadget.h"
#include "cmr_kspace_binning.h"

namespace Gadgetron {

    class CmrCartesianKSpaceBinningCineGadget : public GenericReconGadget
    {
    public:
        typedef GenericReconGadget BaseClass;

        struct Parameters : public BaseClass::Parameters {
            Parameters(const std::string& prefix): BaseClass::Parameters(prefix, "CmrCartesianKSpaceBinningCine") {
                register_parameter("use-multiple-channel-recon", &use_multiple_channel_recon, "Whether to perform multi-channel recon in the raw data step");
                register_parameter("use-nonlinear-binning-recon", &use_nonlinear_binning_recon, "Whether to non-linear recon in the binning step");
                register_parameter("number-of-output-phases", &number_of_output_phases, "Number of output phases after binning");
 
                register_parameter("send-out-raw", &send_out_raw, "Whether to set out raw images");
                register_parameter("send-out-multiple-series-by-slice", &send_out_multiple_series_by_slice, "Whether to set out binning images as multiple series");
 
                register_parameter("arrhythmia-rejector-factor", &arrhythmia_rejector_factor, "If a heart beat RR is not in the range of [ (1-arrhythmiaRejectorFactor)*meanRR (1+arrhythmiaRejectorFactor)*meanRR], it will be rejected");
 
                register_parameter("grappa-kSize-RO", &grappa_kSize_RO, "Raw data recon, kernel size RO");
                register_parameter("grappa-kSize-E1", &grappa_kSize_E1, "Raw data recon, kernel size E1");
                register_parameter("grappa-reg-lamda", &grappa_reg_lamda, "Raw data recon, kernel calibration regularization");
 
                register_parameter("downstream-coil-compression-num-modesKept", &downstream_coil_compression_num_modesKept, "Number of modes kept for down stream coil compression in raw recon step");
                register_parameter("downstream-coil-compression-thres", &downstream_coil_compression_thres, "Threshold for determining number of kept modes in the down stream coil compression");
 
                register_parameter("respiratory-navigator-moco-reg-strength", &respiratory_navigator_moco_reg_strength, "Regularization strength of respiratory moco");
                register_multitoken("respiratory-navigator-moco-iters", &respiratory_navigator_moco_iters, "Number of iterations for respiratory moco");
 
                register_parameter("kspace-binning-interpolate-heart-beat-images", &kspace_binning_interpolate_heart_beat_images, "Whether to interpolate best heart beat images");
                register_parameter("kspace-binning-navigator-acceptance-window", &kspace_binning_navigator_acceptance_window, "Respiratory navigator acceptance window");
                register_parameter("kspace-binning-max-temporal-window", &kspace_binning_max_temporal_window, "Maximally allowed temporal window ratio for binned kspace");
                register_parameter("kspace-binning-minimal-cardiac-phase-width", &kspace_binning_minimal_cardiac_phase_width, "Allowed minimal temporal window for binned kspace, in ms");
 
                register_parameter("kspace-binning-moco-reg-strength", &kspace_binning_moco_reg_strength, "Regularization strength of binning moco");
                register_multitoken("kspace-binning-moco-iters", &kspace_binning_moco_iters, "Number of iterations for binning moco");
 
                register_parameter("kspace-binning-kSize-RO", &kspace_binning_kSize_RO, "Binned kspace recon, kernel size RO");
                register_parameter("kspace-binning-kSize-E1", &kspace_binning_kSize_E1, "Binned kspace recon, kernel size E1");
                register_parameter("kspace-binning-reg-lamda", &kspace_binning_reg_lamda, "Binned kspace recon, kernel calibration regularization");
                register_parameter("kspace-binning-linear-iter-max", &kspace_binning_linear_iter_max, "Binned kspace recon, maximal number of iterations, linear recon");
                register_parameter("kspace-binning-linear-iter-thres", &kspace_binning_linear_iter_thres, "Binned kspace recon, iteration threshold, linear recon");
                register_parameter("kspace-binning-nonlinear-iter-max", &kspace_binning_nonlinear_iter_max, "Binned kspace recon, maximal number of iterations, non-linear recon");
                register_parameter("kspace-binning-nonlinear-iter-thres", &kspace_binning_nonlinear_iter_thres, "Binned kspace recon, iteration threshold, non-linear recon");
                register_parameter("kspace-binning-nonlinear-data-fidelity-lamda", &kspace_binning_nonlinear_data_fidelity_lamda, "Binned kspace recon, strength of data fidelity term, non-linear recon");
                register_parameter("kspace-binning-nonlinear-image-reg-lamda", &kspace_binning_nonlinear_image_reg_lamda, "Binned kspace recon, strength of image term, non-linear recon");
                register_parameter("kspace-binning-nonlinear-reg-N-weighting-ratio", &kspace_binning_nonlinear_reg_N_weighting_ratio, "Binned kspace recon, regularization weighting ratio along the N dimension, non-linear recon");
                register_parameter("kspace-binning-nonlinear-reg-use-coil-sen-map", &kspace_binning_nonlinear_reg_use_coil_sen_map, "Binned kspace recon, whether to use coil map, non-linear recon");
                register_parameter("kspace-binning-nonlinear-reg-with-approx-coeff", &kspace_binning_nonlinear_reg_with_approx_coeff, "Binned kspace recon, whether to keep approximal coefficients, non-linear recon");
                register_parameter("kspace-binning-nonlinear-reg-wav-name", &kspace_binning_nonlinear_reg_wav_name, "Binned kspace recon, wavelet name, non-linear recon");
 
                register_parameter("time-tick", &time_tick, "Time tick in ms");
                register_multitoken("kspace-binning-processed-slices", &kspace_binning_processed_slices, "If set, these slices will be processed");
            }

            // parameters for workflow
            bool use_multiple_channel_recon = true;
            bool use_nonlinear_binning_recon = true;
            int number_of_output_phases = 30;

            bool send_out_raw = false;
            bool send_out_multiple_series_by_slice = false;

            // parameters for raw image reconstruction
            float arrhythmia_rejector_factor = 0.25;

            int grappa_kSize_RO = 5;
            int grappa_kSize_E1 = 4;
            double grappa_reg_lamda = 0.0005;

            size_t downstream_coil_compression_num_modesKept = 0;
            double downstream_coil_compression_thres = 0.01;

            // parameters for kspace binning
            double respiratory_navigator_moco_reg_strength = 6.0;
            std::vector<unsigned int> respiratory_navigator_moco_iters = {1, 32, 100, 100};

            bool kspace_binning_interpolate_heart_beat_images = true;
            double kspace_binning_navigator_acceptance_window = 0.65;
            double kspace_binning_max_temporal_window = 2.0;
            double kspace_binning_minimal_cardiac_phase_width = 25.0;

            double kspace_binning_moco_reg_strength = 12.0;
            std::vector<unsigned int> kspace_binning_moco_iters = {24, 64, 100, 100, 100};

            // parameters for recon on binned kspace
            int kspace_binning_kSize_RO = 7;
            int kspace_binning_kSize_E1 = 7;
            double kspace_binning_reg_lamda = 0.005;
            // linear recon step
            size_t kspace_binning_linear_iter_max = 90;
            double kspace_binning_linear_iter_thres = 0.0015;
            // Non-linear recon step
            size_t kspace_binning_nonlinear_iter_max = 25;
            double kspace_binning_nonlinear_iter_thres = 0.004;
            double kspace_binning_nonlinear_data_fidelity_lamda = 1.0;
            double kspace_binning_nonlinear_image_reg_lamda = 0.00015;
            double kspace_binning_nonlinear_reg_N_weighting_ratio = 10.0;
            bool kspace_binning_nonlinear_reg_use_coil_sen_map = false;
            bool kspace_binning_nonlinear_reg_with_approx_coeff = true;
            std::string kspace_binning_nonlinear_reg_wav_name = "db1";

            float time_tick = 2.5;

            // for debug
            std::vector<unsigned int> kspace_binning_processed_slices = {};
        };

        CmrCartesianKSpaceBinningCineGadget(const Core::MrdContext &context, const Parameters& params);

    protected:
        const Parameters params_;

        // --------------------------------------------------
        // variable for recon
        // --------------------------------------------------

        // binning object
        Gadgetron::CmrKSpaceBinning<float> binning_reconer_;

        // the raw recon results
        // [RO E1 E2 1 N S SLC]
        mrd::ImageArray res_raw_;
        // acqusition time  and trigger time in ms
        // [N S SLC]
        hoNDArray< float > acq_time_raw_;
        hoNDArray< float > cpt_time_raw_;

        // the binning recon results
        // [RO E1 E2 1 binned_N S SLC]
        mrd::ImageArray res_binning_;
        // acqusition time  and trigger time in ms for binned images
        // [binned_N S SLC]
        hoNDArray< float > acq_time_binning_;
        hoNDArray< float > cpt_time_binning_;

        // if true, every slice will be sent out as separate series
        bool send_out_multiple_series_by_slice_;

        // --------------------------------------------------
        // gadget functions
        // --------------------------------------------------
        // default interface function
        void process(Core::InputChannel< mrd::ReconData > &in, Core::OutputChannel &out) override;

        // --------------------------------------------------
        // recon step functions
        // --------------------------------------------------
        virtual void perform_binning(mrd::ReconAssembly& recon_bit, size_t encoding);

        // create binning image header
        void create_binning_image_headers_from_raw();

        // set the time stamps
        void set_time_stamps(mrd::ImageArray& res, hoNDArray< float >& acq_time, hoNDArray< float >& cpt_time);

        // --------------------------------------------------
        // overload functions
        // --------------------------------------------------
        // send out the recon results
        virtual int prep_image_header_send_out(mrd::ImageArray& res, size_t n, size_t s, size_t slc, size_t encoding, int series_num, const std::string& data_role);
    };
}
