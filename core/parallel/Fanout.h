#pragma once

#include <map>
#include <memory>

#include "Branch.h"

#include "Channel.h"

namespace Gadgetron::Core::Parallel {

    /** TODO: Move to MR-specific location! */
    template<class ...ARGS>
    class Fanout : public MRBranch<ARGS...> {
    public:
        using MRBranch<ARGS...>::MRBranch;
        void process(InputChannel<ARGS...> &, std::map<std::string, OutputChannel>) override;
    };

    using AcquisitionFanout = Core::Parallel::Fanout<mrd::Acquisition>;
    using WaveformFanout = Core::Parallel::Fanout<mrd::WaveformUint32>;
    using ImageFanout = Core::Parallel::Fanout<mrd::AnyImage>;
}

#include "Fanout.hpp"
