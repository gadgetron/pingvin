//
// Created by dchansen on 2/21/19.
//

#include "PureCmrCartesianKSpaceBinningCineGadget.h"
#include <boost/range/algorithm/copy.hpp>

namespace {
    using namespace Gadgetron;
    void set_time_stamps(
        mrd::ImageArray& res, const hoNDArray<float>& acq_time, const hoNDArray<float>& cpt_time, float time_tick) {

        size_t N   = res.headers.get_size(0);
        size_t S   = res.headers.get_size(1);
        size_t SLC = res.headers.get_size(2);

        size_t n, s, slc;
        for (slc = 0; slc < SLC; slc++) {
            for (s = 0; s < S; s++) {
                for (n = 0; n < N; n++) {
                    res.headers(n, s, slc).acquisition_time_stamp = (uint32_t)lround(acq_time(n, s, slc) / time_tick);
                    res.headers(n, s, slc).physiology_time_stamp[0]
                        = (uint32_t)lround(cpt_time(n, s, slc) / time_tick);
                }
            }
        }
    }

}

mrd::ImageArray Gadgetron::PureCmrCartesianKSpaceBinningCineGadget::process_function(mrd::ReconData args) const
{
    if (args.buffers.size() > 1)
        throw std::runtime_error("Only single encoding space supported");
    size_t encoding = 0;
    auto result     = perform_binning(args.buffers[encoding], encoding);
    set_image_header(args.buffers[encoding], result.image, encoding);
    set_time_stamps(result.image, result.acquisition_time, result.capture_time, time_tick);
    prepare_image_array(result.image, 0, 2, GADGETRON_IMAGE_RETRO);
    return std::move(result.image);
}
CmrKSpaceBinning<float> PureCmrCartesianKSpaceBinningCineGadget::create_binner() const {
    auto binner = CmrKSpaceBinning<float>{};

    //    binner.perform_timing_ = this->perform_timing;
    //    binner.verbose_        = this->verbose;

    binner.use_multiple_channel_recon_         = this->use_multiple_channel_recon;
    binner.use_paralell_imaging_binning_recon_ = true;
    binner.use_nonlinear_binning_recon_        = this->use_nonlinear_binning_recon;

    binner.estimate_respiratory_navigator_          = true;
    binner.respiratory_navigator_moco_reg_strength_ = this->respiratory_navigator_moco_reg_strength;
    binner.respiratory_navigator_moco_iters_        = this->respiratory_navigator_moco_iters;

    binner.time_tick_                  = this->time_tick;
    binner.trigger_time_index_         = 0;
    binner.arrhythmia_rejector_factor_ = this->arrhythmia_rejector_factor;

    binner.grappa_kSize_RO_                           = this->grappa_kSize_RO;
    binner.grappa_kSize_E1_                           = this->grappa_kSize_E1;
    binner.grappa_reg_lamda_                          = this->grappa_reg_lamda;
    binner.downstream_coil_compression_num_modesKept_ = this->downstream_coil_compression_num_modesKept;
    binner.downstream_coil_compression_thres_         = this->downstream_coil_compression_thres;

    binner.kspace_binning_interpolate_heart_beat_images_ = this->kspace_binning_interpolate_heart_beat_images;
    binner.kspace_binning_navigator_acceptance_window_   = this->kspace_binning_navigator_acceptance_window;

    binner.kspace_binning_moco_reg_strength_ = this->kspace_binning_moco_reg_strength;
    binner.kspace_binning_moco_iters_        = this->kspace_binning_moco_iters;

    binner.kspace_binning_max_temporal_window_             = this->kspace_binning_max_temporal_window;
    binner.kspace_binning_minimal_cardiac_phase_width_     = this->kspace_binning_minimal_cardiac_phase_width;
    binner.kspace_binning_kSize_RO_                        = this->kspace_binning_kSize_RO;
    binner.kspace_binning_kSize_E1_                        = this->kspace_binning_kSize_E1;
    binner.kspace_binning_reg_lamda_                       = this->kspace_binning_reg_lamda;
    binner.kspace_binning_linear_iter_max_                 = this->kspace_binning_linear_iter_max;
    binner.kspace_binning_linear_iter_thres_               = this->kspace_binning_linear_iter_thres;
    binner.kspace_binning_nonlinear_iter_max_              = this->kspace_binning_nonlinear_iter_max;
    binner.kspace_binning_nonlinear_iter_thres_            = this->kspace_binning_nonlinear_iter_thres;
    binner.kspace_binning_nonlinear_data_fidelity_lamda_   = this->kspace_binning_nonlinear_data_fidelity_lamda;
    binner.kspace_binning_nonlinear_image_reg_lamda_       = this->kspace_binning_nonlinear_image_reg_lamda;
    binner.kspace_binning_nonlinear_reg_N_weighting_ratio_ = this->kspace_binning_nonlinear_reg_N_weighting_ratio;
    binner.kspace_binning_nonlinear_reg_use_coil_sen_map_  = this->kspace_binning_nonlinear_reg_use_coil_sen_map;
    binner.kspace_binning_nonlinear_reg_with_approx_coeff_ = this->kspace_binning_nonlinear_reg_with_approx_coeff;
    binner.kspace_binning_nonlinear_reg_wav_name_          = this->kspace_binning_nonlinear_reg_wav_name;

    return binner;
}
PureCmrCartesianKSpaceBinningCineGadget::BinningResult PureCmrCartesianKSpaceBinningCineGadget::perform_binning(
    mrd::ReconAssembly recon_bit, size_t encoding) const {
    size_t RO  = recon_bit.data.data.get_size(0);
    size_t E1  = recon_bit.data.data.get_size(1);
    size_t E2  = recon_bit.data.data.get_size(2);
    size_t CHA = recon_bit.data.data.get_size(3);
    size_t N   = recon_bit.data.data.get_size(4);
    size_t S   = recon_bit.data.data.get_size(5);
    size_t SLC = recon_bit.data.data.get_size(6);

    size_t binned_N = number_of_output_phases;

    auto binner = create_binner();
    BinningResult result;
    result.image.data      = hoNDArray<std::complex<float>>(RO, E1, E2, 1, binned_N, S, SLC);
    result.acquisition_time = hoNDArray<float>(N, S, SLC);
    result.capture_time     = hoNDArray<float>(N, S, SLC);

    for (size_t slc = 0; slc < SLC; slc++) {
        std::stringstream os;

        // set up the binning object
        binner.binning_obj_.data_.create(
            RO, E1, CHA, N, S, recon_bit.data.data.begin() + slc * RO * E1 * CHA * N * S);
        binner.binning_obj_.sampling_ = recon_bit.data.sampling;
        binner.binning_obj_.headers_.create(E1, N, S, recon_bit.data.headers.begin() + slc * E1 * N * S);

        binner.binning_obj_.output_N_        = binned_N;
        binner.binning_obj_.accel_factor_E1_ = acceFactorE1_[encoding];
        binner.binning_obj_.random_sampling_
            = (calib_mode_[encoding] != mrd::CalibrationMode::kEmbedded && calib_mode_[encoding] != mrd::CalibrationMode::kInterleaved
                && calib_mode_[encoding] != mrd::CalibrationMode::kSeparate && calib_mode_[encoding] != mrd::CalibrationMode::kNoacceleration);

        binner.process_binning_recon();

        std::copy_n(binner.binning_obj_.complex_image_binning_.data(),
            binner.binning_obj_.complex_image_binning_.get_number_of_bytes(),
            result.image.data.data() + slc * RO * E1 * N * S * SLC);

        for (size_t s = 0; s < S; s++) {

            for (size_t n = 0; n < binned_N; n++) {
                result.acquisition_time(n, s, slc) = binner.binning_obj_.phs_time_stamp_(n, s);
                result.capture_time(n, s, slc)     = binner.binning_obj_.mean_RR_ * binner.binning_obj_.desired_cpt_[n];
            }
        }
    }
    return result;
}

PureCmrCartesianKSpaceBinningCineGadget::PureCmrCartesianKSpaceBinningCineGadget(
    const Core::Context& context, const Core::GadgetProperties& props)
    : PureGadget(context,props) {
    auto h    = context.header;
    size_t NE = h.encoding.size();

    acceFactorE1_.resize(NE, 1);
    acceFactorE2_.resize(NE, 1);
    calib_mode_.resize(NE, mrd::CalibrationMode::kNoacceleration);
    space_matrix_offset_E1_.resize(NE, 0);
    space_matrix_offset_E2_.resize(NE, 0);

    for (size_t e = 0; e < h.encoding.size(); e++) {

        if (!h.encoding[e].parallel_imaging) {
            calib_mode_[e]   = mrd::CalibrationMode::kNoacceleration;
            acceFactorE1_[e] = 1;
            acceFactorE2_[e] = 1;
        } else {
            mrd::ParallelImagingType p_imaging = *h.encoding[e].parallel_imaging;

            acceFactorE1_[e] = p_imaging.acceleration_factor.kspace_encoding_step_1;
            acceFactorE2_[e] = p_imaging.acceleration_factor.kspace_encoding_step_2;

            calib_mode_[e] = mrd::CalibrationMode::kNoacceleration;
            if (acceFactorE1_[e] > 1 || acceFactorE2_[e] > 1) {
                calib_mode_[e] = *p_imaging.calibration_mode;
            }
        }

        bool is_cartesian_sampling = (h.encoding[e].trajectory == mrd::Trajectory::kCartesian);
        bool is_epi_sampling       = (h.encoding[e].trajectory == mrd::Trajectory::kEpi);
        if (is_cartesian_sampling || is_epi_sampling) {
            if (h.encoding[e].encoding_limits.kspace_encoding_step_1.has_value()) {
                space_matrix_offset_E1_[e] = (int)h.encoding[e].encoded_space.matrix_size.y / 2
                                             - (int)h.encoding[e].encoding_limits.kspace_encoding_step_1->center;
            }

            if (h.encoding[e].encoding_limits.kspace_encoding_step_2.has_value()
                && h.encoding[e].encoded_space.matrix_size.z > 1) {
                space_matrix_offset_E2_[e] = (int)h.encoding[e].encoded_space.matrix_size.z / 2
                                             - (int)h.encoding[e].encoding_limits.kspace_encoding_step_2->center;
            }
        }
        // -------------------------------------------------
    }
}

void PureCmrCartesianKSpaceBinningCineGadget::set_image_header(
    const mrd::ReconAssembly& recon_bit, mrd::ImageArray& res, size_t enc) const {
    size_t RO  = res.data.get_size(0);
    size_t E1  = res.data.get_size(1);
    size_t E2  = res.data.get_size(2);
    size_t CHA = res.data.get_size(3);
    size_t N   = res.data.get_size(4);
    size_t S   = res.data.get_size(5);
    size_t SLC = res.data.get_size(6);

    GADGET_CHECK_THROW(N == recon_bit.data.headers.get_size(2));
    GADGET_CHECK_THROW(S == recon_bit.data.headers.get_size(3));

    res.headers.create(N, S, SLC);
    res.meta.create(N, S, SLC);

    size_t n, s, slc;

    for (slc = 0; slc < SLC; slc++) {
        for (s = 0; s < S; s++) {
            for (n = 0; n < N; n++) {
                size_t header_E1 = recon_bit.data.headers.get_size(0);
                size_t header_E2 = recon_bit.data.headers.get_size(1);

                // for every kspace, find the recorded header which is closest to the kspace center [E1/2 E2/2]
                mrd::AcquisitionHeader acq_header;

                long long bestE1 = E1 + 1;
                long long bestE2 = E2 + 1;

                for (size_t e2 = 0; e2 < header_E2; e2++) {
                    for (size_t e1 = 0; e1 < header_E1; e1++) {
                        const mrd::AcquisitionHeader& curr_header = recon_bit.data.headers(e1, e2, n, s, slc);

                        long long e1_in_bucket = curr_header.idx.kspace_encode_step_1.value_or(0) + space_matrix_offset_E1_[enc];

                        if (E2 > 1) {
                            long long e2_in_bucket
                                = curr_header.idx.kspace_encode_step_2.value_or(0) + space_matrix_offset_E2_[enc];

                            if (std::abs(e1_in_bucket - (long long)(E1 / 2)) < bestE1
                                && std::abs(e2_in_bucket - (long long)(E2 / 2)) < bestE2) {
                                bestE1 = std::abs(e1_in_bucket - (long long)E1 / 2);
                                bestE2 = std::abs(e2_in_bucket - (long long)E2 / 2);

                                acq_header = curr_header;
                            }
                        } else {
                            if (std::abs(e1_in_bucket - (long long)(E1 / 2)) < bestE1) {
                                bestE1 = std::abs(e1_in_bucket - (long long)E1 / 2);

                                acq_header = curr_header;
                            }
                        }
                    }
                }

                mrd::ImageHeader& im_header = res.headers(n, s, slc);
                mrd::ImageMeta& meta    = res.meta(n, s, slc);

                /** TODO: mrd::ImageFlags != mrd::AcquisitionFlags... */
                // im_header.flags           = acq_header.flags;
                im_header.measurement_uid = acq_header.measurement_uid;

                im_header.field_of_view[0] = recon_bit.data.sampling.recon_fov.x;
                im_header.field_of_view[1] = recon_bit.data.sampling.recon_fov.y;
                im_header.field_of_view[2] = recon_bit.data.sampling.recon_fov.z;

                im_header.position = acq_header.position;
                im_header.col_dir = acq_header.read_dir;
                im_header.line_dir = acq_header.phase_dir;
                im_header.slice_dir = acq_header.slice_dir;
                im_header.patient_table_position = acq_header.patient_table_position;

                im_header.average    = acq_header.idx.average;
                im_header.slice      = acq_header.idx.slice;
                im_header.contrast   = acq_header.idx.contrast;
                im_header.phase      = acq_header.idx.phase;
                im_header.repetition = acq_header.idx.repetition;
                im_header.set        = acq_header.idx.set;

                im_header.acquisition_time_stamp = acq_header.acquisition_time_stamp;

                im_header.physiology_time_stamp = acq_header.physiology_time_stamp;

                im_header.image_type         = mrd::ImageType::kMagnitude;
                im_header.image_index        = (uint16_t)(n + s * N + slc * N * S);
                im_header.image_series_index = 0;

                im_header.user_int = acq_header.user_int;
                im_header.user_float = acq_header.user_float;

                meta["encoding"] = { (long)enc };

                auto& encoded_fov = recon_bit.data.sampling.encoded_fov;
                meta["encoding_FOV"] = { encoded_fov.x, encoded_fov.y, encoded_fov.z };

                auto& recon_fov = recon_bit.data.sampling.recon_fov;
                meta["recon_FOV"] = { recon_fov.x, recon_fov.y, recon_fov.z };

                auto& encoded_matrix = recon_bit.data.sampling.encoded_matrix;
                meta["encoded_matrix"] = { (long)encoded_matrix.x, (long)encoded_matrix.y, (long)encoded_matrix.z };

                auto& recon_matrix = recon_bit.data.sampling.recon_matrix;
                meta["recon_matrix"] = { (long)recon_matrix.x, (long)recon_matrix.y, (long)recon_matrix.z };

                auto& sampling_limits = recon_bit.data.sampling.sampling_limits;
                meta["sampling_limits_RO"] = { (long)sampling_limits.kspace_encoding_step_0.minimum, (long)sampling_limits.kspace_encoding_step_0.center, (long)sampling_limits.kspace_encoding_step_0.maximum };

                meta["sampling_limits_E1"] = { (long)sampling_limits.kspace_encoding_step_1.minimum, (long)sampling_limits.kspace_encoding_step_1.center, (long)sampling_limits.kspace_encoding_step_1.maximum };

                meta["sampling_limits_E2"] = { (long)sampling_limits.kspace_encoding_step_2.minimum, (long)sampling_limits.kspace_encoding_step_2.center, (long)sampling_limits.kspace_encoding_step_2.maximum };

                auto &hdrsrc = res.headers(n, s, slc);
                auto& position = hdrsrc.position;
                meta["PatientPosition"] = { (double)position[0], (double)position[1], (double)position[2] };

                auto& col_dir = hdrsrc.col_dir;
                meta["read_dir"] = { (double)col_dir[0], (double)col_dir[1], (double)col_dir[2] };

                auto& line_dir = hdrsrc.line_dir;
                meta["phase_dir"] = { (double)line_dir[0], (double)line_dir[1], (double)line_dir[2] };

                auto& slice_dir = hdrsrc.slice_dir;
                meta["slice_dir"] = { (double)slice_dir[0], (double)slice_dir[1], (double)slice_dir[2] };

                auto &patient_table_position = hdrsrc.patient_table_position;
                meta["patient_table_position"] = { (double)patient_table_position[0], (double)patient_table_position[1], (double)patient_table_position[2] };

                meta["acquisition_time_stamp"] = { (long)hdrsrc.acquisition_time_stamp.value_or(0) };

                auto& physio_times = hdrsrc.physiology_time_stamp;
                meta["physiology_time_stamp"] = { (long)physio_times[0], (long)physio_times[1], (long)physio_times[2] };
            }
        }
    }
}
