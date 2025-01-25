#include "EPIReconXGadget.h"
#include "hoNDArray_utils.h"
#include "hoNDFFT.h"

#ifdef USE_OMP
#include "omp.h"
#endif // USE_OMP

namespace Gadgetron {

    EPIReconXGadget::EPIReconXGadget(const Core::Context& context, const Core::GadgetProperties& props)
        : ChannelGadget(context, props)
    {
        auto& h = context.header;

        if (h.encoding.empty()) {
            GDEBUG("This Gadget needs an encoding description\n");
            GADGET_THROW("No encoding spaces found in header");
        }

        GDEBUG("Number of encoding spaces = %d\n", h.encoding.size());

        // Get the encoding space and trajectory description
        mrd::EncodingSpaceType e_space = h.encoding[0].encoded_space;
        mrd::EncodingSpaceType r_space = h.encoding[0].recon_space;
        mrd::EncodingLimitsType e_limits = h.encoding[0].encoding_limits;
        mrd::TrajectoryDescriptionType traj_desc;

        if (h.encoding[0].trajectory_description) {
            traj_desc = *h.encoding[0].trajectory_description;
        } else {
            GADGET_THROW("Trajectory description missing");
        }

        if (traj_desc.identifier != "ConventionalEPI") {
            GADGET_THROW("Expected trajectory description identifier 'ConventionalEPI', not found.");
        }

        // Primary encoding space is for EPI
        reconx.encodeNx_ = e_space.matrix_size.x;
        reconx.encodeFOV_ = e_space.field_of_view_mm.x;
        reconx.reconNx_ = r_space.matrix_size.x;
        reconx.reconFOV_ = r_space.field_of_view_mm.x;

        // TODO: we need a flag that says it's a balanced readout.
        for (const auto& param : traj_desc.user_parameter_long) {
            if (param.name == "rampUpTime") {
                reconx.rampUpTime_ = param.value;
            } else if (param.name == "rampDownTime") {
                reconx.rampDownTime_ = param.value;
            } else if (param.name == "flatTopTime") {
                reconx.flatTopTime_ = param.value;
            } else if (param.name == "acqDelayTime") {
                reconx.acqDelayTime_ = param.value;
            } else if (param.name == "numSamples") {
                reconx.numSamples_ = param.value;
            }
        }

        for (const auto& param : traj_desc.user_parameter_double) {
            if (param.name == "dwellTime") {
                reconx.dwellTime_ = param.value;
            }
        }

        // If the flat top time is not set in the header, then we assume that rampSampling is off
        // and we set the flat top time from the number of samples and the dwell time.
        if (reconx.flatTopTime_ == 0) {
            reconx.flatTopTime_ = reconx.dwellTime_ * reconx.numSamples_;
        }

        // Compute the trajectory
        reconx.computeTrajectory();

        // Second encoding space is an even readout for PAT REF e.g. FLASH
        if (h.encoding.size() > 1) {
            mrd::EncodingSpaceType e_space2 = h.encoding[1].encoded_space;
            mrd::EncodingSpaceType r_space2 = h.encoding[1].recon_space;
            reconx_other.encodeNx_ = r_space2.matrix_size.x;
            reconx_other.encodeFOV_ = r_space2.field_of_view_mm.x;
            reconx_other.reconNx_ = r_space2.matrix_size.x;
            reconx_other.reconFOV_ = r_space2.field_of_view_mm.x;
            reconx_other.numSamples_ = e_space2.matrix_size.x;
            oversamplng_ratio2_ = static_cast<float>(e_space2.matrix_size.x) / r_space2.matrix_size.x;
            reconx_other.dwellTime_ = 1.0;
            reconx_other.computeTrajectory();
        }

#ifdef USE_OMP
        omp_set_num_threads(1);
#endif // USE_OMP
    }

    void EPIReconXGadget::process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out)
    {
        for (auto acq : input) {
            auto& hdr_in = acq.head;
            auto& data_in = acq.data;

            mrd::AcquisitionHeader hdr_out;
            hoNDArray<std::complex<float>> data_out;

            data_out.create(reconx.reconNx_, data_in.get_size(1));

            // Switch the reconstruction based on the encoding space (e.g. for FLASH Calibration)
            if (hdr_in.encoding_space_ref == 0) {
                reconx.apply(hdr_in, data_in, hdr_out, data_out);
            } else {
                if (reconx_other.encodeNx_ > data_in.get_size(0) / oversamplng_ratio2_) {
                    reconx_other.encodeNx_ = static_cast<int>(data_in.get_size(0) / oversamplng_ratio2_);
                    reconx_other.computeTrajectory();
                }

                if (reconx_other.reconNx_ > data_in.get_size(0) / oversamplng_ratio2_) {
                    reconx_other.reconNx_ = static_cast<int>(data_in.get_size(0) / oversamplng_ratio2_);
                }

                if (reconx_other.numSamples_ > data_in.get_size(0)) {
                    reconx_other.numSamples_ = data_in.get_size(0);
                    reconx_other.computeTrajectory();
                }

                reconx_other.apply(hdr_in, data_in, hdr_out, data_out);
            }

            // Replace the contents of acq with the new header and the contents of m2 with the new data
            acq.head = hdr_out;
            acq.data = data_out;

            // It is enough to put the first one, since they are linked
            out.push(std::move(acq));
        }
    }

    GADGETRON_GADGET_EXPORT(EPIReconXGadget)

} // namespace Gadgetron


