#include "EPIPackNavigatorGadget.h"

namespace Gadgetron {

EPIPackNavigatorGadget::EPIPackNavigatorGadget(const Core::Context& context, const Core::GadgetProperties& props)
    : ChannelGadget(context, props)
{
    auto& h = context.header;
    if (h.encoding.size() == 0) {
        GDEBUG("Number of encoding spaces: %d\n", h.encoding.size());
        GADGET_THROW("This Gadget needs an encoding description\n");
    }

    // Get the encoding space and trajectory description
    mrd::TrajectoryDescriptionType traj_desc;

    if (h.encoding[0].trajectory_description) {
        traj_desc = *h.encoding[0].trajectory_description;
    } else {
        GADGET_THROW("Trajectory description missing");
    }

    if (traj_desc.identifier != "ConventionalEPI") {
        GADGET_THROW("Expected trajectory description identifier 'ConventionalEPI', not found.");
    }

    for (const auto& param : traj_desc.user_parameter_long) {
        if (param.name == "numberOfNavigators") {
            numNavigators_ = param.value;
        }
    }
}

void EPIPackNavigatorGadget::process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out)
{
    // EPI counters for a given shot
    int navNumber_ = -1;
    int epiEchoNumber_ = -1;

    arma::cx_fcube navdata_;

    for (auto acq : input) {
        auto& hdr = acq.head;
        auto& data = acq.data;

        // Pass on the non-EPI data (e.g. FLASH Calibration)
        if (hdr.encoding_space_ref > 0) {
            out.push(std::move(acq));
            continue;
        }

        // We have data from encoding space 0.
        // Make an armadillo matrix of the data
        arma::cx_fmat adata = as_arma_matrix(data);

        // Check to see if the data is a navigator line or an imaging line
        if (hdr.flags.HasFlags(mrd::AcquisitionFlags::kIsPhasecorrData)) {
            // Increment the navigator counter
            navNumber_ += 1;

            // If the number of navigators per shot is exceeded, then
            // we are at the beginning of the next shot
            if (navNumber_ == numNavigators_) {
                navNumber_ = 0;
                epiEchoNumber_ = -1;
            }

            // If we are at the beginning of a shot, then initialize
            if (navNumber_ == 0) {
                // Set the size of the storage array
                navdata_.set_size(adata.n_rows, acq.Coils(), numNavigators_);
            }

            // Store the navigator data
            navdata_.slice(navNumber_) = adata;
        } else {
            // Increment the echo number
            epiEchoNumber_ += 1;

            // Pack navigators into the integer component (c.f. msb) of early epi echo float data
            if (epiEchoNumber_ < numNavigators_) {
                adata = 1.0e2 * adata + arma::round(1.0e6 * navdata_.slice(epiEchoNumber_));
            }

            // Pass on the imaging data
            out.push(std::move(acq));
        }
    }
}

GADGETRON_GADGET_EXPORT(EPIPackNavigatorGadget)
} // namespace Gadgetron
