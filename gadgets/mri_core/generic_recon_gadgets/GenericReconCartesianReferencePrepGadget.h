/** \file   GenericReconCartesianReferencePrepGadget.h
    \brief  This is the class gadget for both 2DT and 3DT cartesian reconstruction to prepare the reference data, working on the ReconData.
    \author Hui Xue
*/

#pragma once

#include "GenericReconBase.h"

#include "hoNDArray_utils.h"
#include "hoNDArray_elemwise.h"

namespace Gadgetron {

    class GenericReconCartesianReferencePrepGadget : public GenericReconDataBase
    {
    public:
        typedef GenericReconDataBase BaseClass;

        struct Parameters : public BaseClass::Parameters {
            Parameters(const std::string &prefix) : BaseClass::Parameters(prefix, "Cartesian Reference Prep") {
                register_parameter("average-all-ref-N", &average_all_ref_N, "Whether to average all N for ref generation");
                register_parameter("average-all-ref-S", &average_all_ref_S, "Whether to average all S for ref generation");
                register_parameter("N-for-ref", &N_for_ref, "If N >= 0, this N will be used for ref preparation");
                register_parameter("S-for-ref", &S_for_ref, "If S >= 0, this S will be used for ref preparation");
                register_flag("ref-fill-into-data-embedded", &ref_fill_into_data_embedded, "If true and calibration is in embedded mode, fill the full sampled data from ref array into the data array");
                register_parameter("prepare-ref-always", &prepare_ref_always, "Whether to prepare ref for every incoming ReconData");
            }

            /// ref preparation
            /// whether to average all N for ref generation
            /// for the interleaved mode, the sampling times will be counted and used for averaging
            /// it is recommended to set N as the interleaved dimension
            bool average_all_ref_N = true;
            /// whether to average all S for ref generation
            bool average_all_ref_S = false;

        /// pick specific N or S for ref, these options overwrites average_all_ref_N and average_all_ref_S
            int N_for_ref = -1;
            int S_for_ref = -1;

            /// some reconstruction will benefit to fill back the ref data into data array for the embedded mode
            bool ref_fill_into_data_embedded = false;

            /// whether to update ref for every incoming ReconData; for some applications, we may want to only compute ref data once
            /// if false, the ref will only be prepared for the first incoming ReconData
            bool prepare_ref_always = true;
        };

        GenericReconCartesianReferencePrepGadget(const Core::MrdContext &context, const Parameters& params);

    protected:
        const Parameters params_;

        // --------------------------------------------------
        // variables for protocol
        // --------------------------------------------------

        /// indicate whether ref has been prepared for an encoding space
        std::vector<bool> ref_prepared_;

        // for every encoding space
        // calibration mode
        std::vector<mrd::CalibrationMode> calib_mode_;

        // --------------------------------------------------
        // variable for recon
        // --------------------------------------------------

        // --------------------------------------------------
        // gadget functions
        // --------------------------------------------------
        // default interface function
        void process(Core::InputChannel<mrd::ReconData> &in, Core::OutputChannel &out) override;
    };
}
