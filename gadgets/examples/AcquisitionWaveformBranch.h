#pragma once

#include "MRParallel.h"

namespace Gadgetron::Examples {

    using AcquisitionOrWaveform = std::variant<mrd::Acquisition, mrd::WaveformUint32>;

    class AcquisitionWaveformBranch : public Core::Parallel::MRBranch<AcquisitionOrWaveform> {
    public:
        AcquisitionWaveformBranch(const Core::MRContext &context, const Core::NodeParameters &params)
            : MRBranch<AcquisitionOrWaveform>(context, params)
        {}

        void process(
                Core::InputChannel<AcquisitionOrWaveform> &,
                std::map<std::string, Core::OutputChannel>
        ) override;
    };
}